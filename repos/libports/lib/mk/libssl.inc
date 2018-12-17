LIBSSL_PORT_DIR = $(call select_from_ports,openssl)

LIBS += libc libcrypto

#SRC_C = s3_lib.c  s3_enc.c s3_cbc.c \
#        t1_lib.c  t1_enc.c \
#        d1_lib.c \
#        d1_srtp.c \
#        ssl_lib.c ssl_cert.c ssl_sess.c \
#        ssl_ciph.c ssl_stat.c ssl_rsa.c \
#        ssl_asn1.c ssl_txt.c \
#        bio_ssl.c ssl_err.c tls_srp.c t1_reneg.c

SRC_C = bio_ssl.c  d1_srtp.c  s3_cbc.c  s3_msg.c    ssl_ciph.c  ssl_init.c  ssl_rsa.c   ssl_txt.c   t1_ext.c    t1_trce.c \
        d1_lib.c   methods.c  s3_enc.c  ssl_asn1.c  ssl_conf.c  ssl_lib.c   ssl_sess.c  ssl_utst.c  t1_lib.c    tls_srp.c \
        d1_msg.c   pqueue.c   s3_lib.c  ssl_cert.c  ssl_err.c   ssl_mcnf.c  ssl_stat.c  t1_enc.c    t1_reneg.c 

SRC_C_record = dtls1_bitmap.o \
               rec_layer_d1.o \
               rec_layer_s3.o \
               ssl3_buffer.o \
               ssl3_record.o
SRC_C_statem = statem_clnt.o \
               statem_dtls.o \
               statem_lib.o \
               statem.o \
               statem_srvr.o
SRC_DIR = record \
          statem

SRC_C += $(foreach dir,$(SRC_DIR),$(addprefix $(dir)/,$(SRC_C_$(dir))))

INC_DIR += $(LIBSSL_PORT_DIR)/include/openssl
INC_DIR += $(LIBSSL_PORT_DIR)/src/lib/openssl
INC_DIR += $(LIBSSL_PORT_DIR)/src/lib/openssl/crypto
INC_DIR += $(LIBSSL_PORT_DIR)/include

ifeq ($(filter-out $(SPECS),arm),)
INC_DIR += $(GENODE_DIR)/repos/libports/src/lib/openssl/spec/arm/openssl/opensslconf.h
endif

ifeq ($(filter-out $(SPECS),x86_32),)
INC_DIR += $(GENODE_DIR)/repos/libports/src/lib/openssl/spec/x86_32/openssl/opensslconf.h
endif

ifeq ($(filter-out $(SPECS),x86_64),)
INC_DIR += $(GENODE_DIR)/repos/libports/src/lib/openssl/spec/x86_64/openssl/opensslconf.h
endif

vpath %.c $(LIBSSL_PORT_DIR)/src/lib/openssl/ssl

SHARED_LIB = yes
