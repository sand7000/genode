/*
 * \brief  Unix emulation environment for Genode
 * \author Norman Feske
 * \date   2011-02-14
 */

/*
 * Copyright (C) 2011-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/*
 * TODO
 *
 * ;- Child class
 * ;- Pass args and env to child
 * ;- Receive args by child
 * ;- run 'echo hello'
 * ;- run 'seq 10'
 * ;- skeleton of noux RPC interface
 * ;- run 'pwd'
 * ;- move _write->LOG to libc plugin
 * ;- stdio (implementation of _write)
 * ;- vfs
 * ;- TAR file system
 * ;- run 'ls -lRa' (dirent syscall)
 * ;- run 'cat' (read syscall)
 * ;- execve
 * ;- fork
 * ;- pipe
 * ;- read init binary from vfs
 * ;- import env into child (execve and fork)
 * ;- shell
 * - debug 'find'
 * - stacked file system infrastructure
 * - TMP file system
 * ;- RAM service using a common quota pool
 */

/* Genode includes */
#include <cap_session/connection.h>
#include <os/config.h>

/* Noux includes */
#include <child.h>
#include <vfs_io_channel.h>
#include <terminal_io_channel.h>
#include <dummy_input_io_channel.h>
#include <pipe_io_channel.h>
#include <root_file_system.h>
#include <tar_file_system.h>


enum { verbose_syscall = false };


namespace Noux {

	static Noux::Child *init_child;

	bool is_init_process(Child *child) { return child == init_child; }
	void init_process_exited() { init_child = 0; }
};


extern "C" void wait_for_continue();


/*****************************
 ** Noux syscall dispatcher **
 *****************************/

bool Noux::Child::syscall(Noux::Session::Syscall sc)
{
	if (verbose_syscall)
		Genode::printf("PID %d -> SYSCALL %s\n",
		               pid(), Noux::Session::syscall_name(sc));

	try {
		switch (sc) {

		case SYSCALL_GETCWD:

			Genode::strncpy(_sysio->getcwd_out.path, _env.pwd(),
			                sizeof(_sysio->getcwd_out.path));
			return true;

		case SYSCALL_WRITE:
			{
				size_t const count_in = _sysio->write_in.count;

				for (size_t count = 0; count != count_in; ) {

					Shared_pointer<Io_channel> io = _lookup_channel(_sysio->write_in.fd);

					if (!io->check_unblock(false, true, false))
						_block_for_io_channel(io);

					/*
					 * 'io->write' is expected to update 'write_out.count'
					 */
					if (io->write(_sysio, count) == false)
						return false;
				}
				return true;
			}

		case SYSCALL_READ:
			{
				Shared_pointer<Io_channel> io = _lookup_channel(_sysio->read_in.fd);

				while (!io->check_unblock(true, false, false))
					_block_for_io_channel(io);

				io->read(_sysio);
				return true;
			}

		case SYSCALL_STAT:
		case SYSCALL_LSTAT: /* XXX implement difference between 'lstat' and 'stat' */

			return _vfs->stat(_sysio, Absolute_path(_sysio->stat_in.path,
			                                        _env.pwd()).base());

		case SYSCALL_FSTAT:

			return _lookup_channel(_sysio->fstat_in.fd)->fstat(_sysio);

		case SYSCALL_FCNTL:

			return _lookup_channel(_sysio->fcntl_in.fd)->fcntl(_sysio);

		case SYSCALL_OPEN:
			{
				Absolute_path absolute_path(_sysio->open_in.path, _env.pwd());

				PINF("open pwd=%s path=%s", _env.pwd(), _sysio->open_in.path);

				/* remember mode only for debug output */
				int const mode = _sysio->open_in.mode;

				Vfs_handle *vfs_handle = _vfs->open(_sysio, absolute_path.base());
				if (!vfs_handle)
					return false;

				Shared_pointer<Io_channel> channel(new Vfs_io_channel(absolute_path.base(), _vfs, vfs_handle),
				                                   Genode::env()->heap());

				_sysio->open_out.fd = add_io_channel(channel);

				PINF("open fd %d for \"%s\" with mode %o",
				     _sysio->open_out.fd, absolute_path.base(), mode);
				return true;
			}

		case SYSCALL_CLOSE:
			{
				remove_io_channel(_sysio->close_in.fd);
				return true;
			}

		case SYSCALL_IOCTL:

			return _lookup_channel(_sysio->ioctl_in.fd)->ioctl(_sysio);

		case SYSCALL_DIRENT:

			return _lookup_channel(_sysio->dirent_in.fd)->dirent(_sysio);

		case SYSCALL_FCHDIR:

			return _lookup_channel(_sysio->fchdir_in.fd)->fchdir(_sysio, &_env);

		case SYSCALL_EXECVE:
			{
				char const *filename = _sysio->execve_in.filename;

				/*
				 * Deserialize environment variable buffer into a
				 * null-terminated string. The source env buffer contains a
				 * list of strings separated by single 0 characters. Each
				 * string has the form "name=value" (w/o the quotes). The end
				 * of the list is marked by an additional 0 character. The
				 * resulting string is a null-terminated string containing a
				 * comma-separated list of environment variables.
				 *
				 * In the following loop, 'i' is the index into the source
				 * buffer, 'j' is the index into the destination buffer, 'env'
				 * is the destination.
				 */
				char env[Sysio::ENV_MAX_LEN];
				for (unsigned i = 0, j = 0; i < Sysio::ENV_MAX_LEN && _sysio->execve_in.env[i]; )
				{
					char const *src = &_sysio->execve_in.env[i];

					/* prepend a comma in front of each entry except for the first one */
					if (i) {
						snprintf(env + j, sizeof(env) - j, ",");
						j++;
					}

					snprintf(env + j, sizeof(env) - j, "%s", src);

					/* skip null separator in source string */
					i += strlen(src) + 1;
					j += strlen(src);
				}

				Child *child = new Child(filename,
				                         parent(),
				                         pid(),
				                         _sig_rec,
				                         _vfs,
				                         Args(_sysio->execve_in.args,
				                              sizeof(_sysio->execve_in.args)),
				                         env,
				                         _env.pwd(),
				                         _cap_session,
				                         _parent_services,
				                         _resources.ep,
				                         false);

				/* replace ourself by the new child at the parent */
				parent()->remove(this);
				parent()->insert(child);

				_assign_io_channels_to(child);

				/* signal main thread to remove ourself */
				Genode::Signal_transmitter(_execve_cleanup_context_cap).submit();

				/* start executing the new process */
				child->start();

				/* this child will be removed by the execve_finalization_dispatcher */
				return true;
			}

		case SYSCALL_SELECT:
			{
				Sysio::Select_fds &in_fds = _sysio->select_in.fds;

				/*
				 * Block for one action of the watched file descriptors
				 */
				for (;;) {

					/*
					 * Check I/O channels of specified file descriptors for
					 * unblock condition. Return if one I/O channel satisfies
					 * the condition.
					 */
					for (Genode::size_t i = 0; i < in_fds.total_fds(); i++) {

						int fd = in_fds.array[i];
						if (!fd_in_use(fd)) continue;

						Shared_pointer<Io_channel> io = io_channel_by_fd(fd);

						if (io->check_unblock(in_fds.watch_for_rd(i),
						                      in_fds.watch_for_wr(i),
						                      in_fds.watch_for_ex(i))) {

							/*
							 * Return single file descriptor that triggered the
							 * unblocking. For now, only a single file
							 * descriptor is returned on each call of select.
							 */
							Sysio::Select_fds &out_fds = _sysio->select_out.fds;

							out_fds.array[0] = fd;
							out_fds.num_rd   = io->check_unblock(true, false, false);
							out_fds.num_wr   = io->check_unblock(false, true, false);
							out_fds.num_ex   = io->check_unblock(false, false, true);

							return true;
						}
					}

					/*
					 * Return if I/O channel triggered, but timeout exceeded
					 */
					if (_sysio->select_in.timeout.zero()) {
						_sysio->select_out.fds.num_rd = 0;
						_sysio->select_out.fds.num_wr = 0;
						_sysio->select_out.fds.num_ex = 0;

						return true;
					}

					/*
					 * Register ourself at all watched I/O channels
					 *
					 * We instantiate as many notifiers as we have file
					 * descriptors to observe. Each notifier is associated
					 * with the child's blocking semaphore. When any of the
					 * notifiers get woken up, the semaphore gets unblocked.
					 *
					 * XXX However, the semaphore may get unblocked for other
					 *     conditions such as the destruction of the child.
					 *     ...to be done.
					 */
					Wake_up_notifier notifiers[in_fds.total_fds()];

					for (Genode::size_t i = 0; i < in_fds.total_fds(); i++) {
						int fd = in_fds.array[i];
						if (!fd_in_use(fd)) continue;

						Shared_pointer<Io_channel> io = io_channel_by_fd(fd);
						notifiers[i].semaphore = &_blocker;
						io->register_wake_up_notifier(&notifiers[i]);
					}

					/*
					 * Block at barrier except when reaching the timeout
					 */
					_blocker.down();

					/*
					 * Unregister barrier at watched I/O channels
					 */
					for (Genode::size_t i = 0; i < in_fds.total_fds(); i++) {
						int fd = in_fds.array[i];
						if (!fd_in_use(fd)) continue;

						Shared_pointer<Io_channel> io = io_channel_by_fd(fd);
						io->unregister_wake_up_notifier(&notifiers[i]);
					}
				}

				return true;
			}

		case SYSCALL_FORK:
			{
				Genode::addr_t ip              = _sysio->fork_in.ip;
				Genode::addr_t sp              = _sysio->fork_in.sp;
				Genode::addr_t parent_cap_addr = _sysio->fork_in.parent_cap_addr;

				int const new_pid = pid_allocator()->alloc();

				/*
				 * XXX To ease debugging, it would be useful to generate a
				 *     unique name that includes the PID instead of just
				 *     reusing the name of the parent.
				 */
				Child *child = new Child(_child_policy.name(),
				                         this,
				                         new_pid,
				                         _sig_rec,
				                         _vfs,
				                         _args,
				                         _env.env(),
				                         _env.pwd(),
				                         _cap_session,
				                         _parent_services,
				                         _resources.ep,
				                         true);

				Family_member::insert(child);

				_assign_io_channels_to(child);

				/* copy our address space into the new child */
				_resources.rm.replay(child->ram(), child->rm(),
				                     child->ds_registry(), _resources.ep);

				/* start executing the main thread of the new process */
				child->start_forked_main_thread(ip, sp, parent_cap_addr);

				/* activate child entrypoint, thereby starting the new process */
				child->start();

				_sysio->fork_out.pid = new_pid;

				return true;
			}

		case SYSCALL_GETPID:
			{
				_sysio->getpid_out.pid = pid();
				return true;
			}

		case SYSCALL_WAIT4:
			{
				Family_member *exited = _sysio->wait4_in.nohang ? poll4() : wait4();

				if (exited) {
					_sysio->wait4_out.pid    = exited->pid();
					_sysio->wait4_out.status = exited->exit_status();
					Family_member::remove(exited);

					PINF("submit exit signal for PID %d", exited->pid());
					static_cast<Child *>(exited)->submit_exit_signal();

				} else {
					_sysio->wait4_out.pid    = 0;
					_sysio->wait4_out.status = 0;
				}
				return true;
			}

		case SYSCALL_PIPE:
			{
				Shared_pointer<Pipe> pipe(new Pipe, Genode::env()->heap());

				Shared_pointer<Io_channel> pipe_sink(new Pipe_sink_io_channel(pipe, *_sig_rec),
				                                     Genode::env()->heap());
				Shared_pointer<Io_channel> pipe_source(new Pipe_source_io_channel(pipe, *_sig_rec),
				                                       Genode::env()->heap());

				_sysio->pipe_out.fd[0] = add_io_channel(pipe_source);
				_sysio->pipe_out.fd[1] = add_io_channel(pipe_sink);

				return true;
			}

		case SYSCALL_DUP2:
			{
				add_io_channel(io_channel_by_fd(_sysio->dup2_in.fd),
				               _sysio->dup2_in.to_fd);
				return true;
			}

		case SYSCALL_INVALID: break;
		}
	}

	catch (Invalid_fd) {
		_sysio->error.general = Sysio::ERR_FD_INVALID;
		PERR("Invalid file descriptor"); }

	catch (...) { PERR("Unexpected exception"); }

	return false;
}


/**
 * Return name of init process as specified in the config
 */
static char *name_of_init_process()
{
	enum { INIT_NAME_LEN = 128 };
	static char buf[INIT_NAME_LEN];
	Genode::config()->xml_node().sub_node("start").attribute("name").value(buf, sizeof(buf));
	return buf;
}


/**
 * Read command-line arguments of init process from config
 */
static Noux::Args const &args_of_init_process()
{
	static char args_buf[4096];
	static Noux::Args args(args_buf, sizeof(args_buf));

	Genode::Xml_node start_node = Genode::config()->xml_node().sub_node("start");

	try {
		/* the first argument is the program name */
		args.append(name_of_init_process());

		Genode::Xml_node arg_node = start_node.sub_node("arg");
		for (; ; arg_node = arg_node.next("arg")) {
			static char buf[512];
			arg_node.attribute("value").value(buf, sizeof(buf));
			args.append(buf);
		}
	}
	catch (Genode::Xml_node::Nonexistent_sub_node) { }
	catch (Noux::Args::Overrun) { PERR("Argument buffer overrun"); }

	return args;
}


static void quote(char *buf, Genode::size_t buf_len)
{
	/*
	 * Make sure to leave space at the end of buffer for the finishing '"' and
	 * the null-termination.
	 */
	char c = '"';
	unsigned i = 0;
	for (; c && (i + 2 < buf_len); i++)
	{
		/*
		 * So shouldn't this have a special case for '"' characters inside the
		 * string? This is actually not needed because such a string could
		 * never be constructed via the XML config anyway. You can sneak in '"'
		 * characters only by quoting them in the XML file. Then, however, they
		 * are already quoted.
		 */
		char next_c = buf[i];
		buf[i] = c;
		c = next_c;
	}
	buf[i + 0] = '"';
	buf[i + 1] = 0;
}


/**
 * Return string containing the environment variables of init
 *
 * The string is formatted according to the 'Genode::Arg_string' rules.
 */
static char const *env_string_of_init_process()
{
	static char env_buf[4096];

	Genode::Arg_string::set_arg(env_buf, sizeof(env_buf), "PWD", "\"/\"");

	/* read environment variables for init process from config */
	Genode::Xml_node start_node = Genode::config()->xml_node().sub_node("start");
	try {
		Genode::Xml_node arg_node = start_node.sub_node("env");
		for (; ; arg_node = arg_node.next("env")) {
			static char name_buf[256], value_buf[256];

			arg_node.attribute("name").value(name_buf, sizeof(name_buf));
			arg_node.attribute("value").value(value_buf, sizeof(value_buf));
			quote(value_buf, sizeof(value_buf));

			Genode::Arg_string::set_arg(env_buf, sizeof(env_buf),
			                            name_buf, value_buf);
		}
	}
	catch (Genode::Xml_node::Nonexistent_sub_node) { }

	return env_buf;
}


Noux::Pid_allocator *Noux::pid_allocator()
{
	static Noux::Pid_allocator inst;
	return &inst;
}


void *operator new (Genode::size_t size) {
	return Genode::env()->heap()->alloc(size); }


int main(int argc, char **argv)
{
	using namespace Noux;
	PINF("--- noux started ---");

	/* look for dynamic linker */
	try {
		static Genode::Rom_connection rom("ld.lib.so");
		Genode::Process::dynamic_linker(rom.dataspace());
	} catch (...) { }

	/* whitelist of service requests to be routed to the parent */
	static Genode::Service_registry parent_services;
	char const *service_names[] = { "LOG", "ROM", "Timer", 0 };
	for (unsigned i = 0; service_names[i]; i++)
		parent_services.insert(new Genode::Parent_service(service_names[i]));

	static Genode::Cap_connection cap;

	/* initialize virtual file system */
	static Vfs vfs;

	try {
		Genode::Xml_node fs = Genode::config()->xml_node().sub_node("fstab").sub_node();
		for (; ; fs = fs.next()) {
			if (fs.has_type("tar"))
				vfs.add_file_system(new Tar_file_system(fs));
		}
	} catch (Genode::Xml_node::Nonexistent_sub_node) { }

	/*
	 * Entrypoint used to virtualize child resources such as RAM, RM
	 */
	enum { STACK_SIZE = 1024*sizeof(long) };
	static Genode::Rpc_entrypoint resources_ep(&cap, STACK_SIZE, "noux_rsc_ep");

	/* create init process */
	static Genode::Signal_receiver sig_rec;

	init_child = new Noux::Child(name_of_init_process(),
	                             0,
	                             pid_allocator()->alloc(),
	                             &sig_rec,
	                             &vfs,
	                             args_of_init_process(),
	                             env_string_of_init_process(),
	                             "/",
	                             &cap,
	                             parent_services,
	                             resources_ep,
	                             false);

	static Terminal::Connection terminal;

	/*
	 * I/O channels must be dynamically allocated to handle cases where the
	 * init program closes one of these.
	 */
	typedef Terminal_io_channel Tio; /* just a local abbreviation */
	Shared_pointer<Io_channel>
		channel_0(new Tio(terminal, Tio::STDIN,  sig_rec), Genode::env()->heap()),
		channel_1(new Tio(terminal, Tio::STDOUT, sig_rec), Genode::env()->heap()),
		channel_2(new Tio(terminal, Tio::STDERR, sig_rec), Genode::env()->heap());

	init_child->add_io_channel(channel_0, 0);
	init_child->add_io_channel(channel_1, 1);
	init_child->add_io_channel(channel_2, 2);

	init_child->start();

	/* handle asynchronous events */
	while (init_child) {

		Genode::Signal signal = sig_rec.wait_for_signal();

		Signal_dispatcher *dispatcher =
			static_cast<Signal_dispatcher *>(signal.context());

		for (int i = 0; i < signal.num(); i++)
			dispatcher->dispatch();
	}

	PINF("-- exiting noux ---");
	return 0;
}
