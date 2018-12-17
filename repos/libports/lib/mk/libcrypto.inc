OPENSSL_PORT_DIR := $(call select_from_ports,openssl)
LIBCRYPTO_DIR    := $(OPENSSL_PORT_DIR)/src/lib/openssl/crypto

SHARED_LIB = yes

LIBS += libc zlib

CC_OPT += -DDSO_DLFCN -DHAVE_DLFCN_H -Wa,--noexecstack -DL_ENDIAN -DTERMIOS
#CC_OPT += -DDSO_DLFCN -DHAVE_DLFCN_H -Wa,--noexecstack -DL_ENDIAN -DTERMIOS \
#          -DOPENSSL_NO_ASM -DGETPID_IS_MEANINGLESS
CC_OPT += -DRAND_GENODE
#CC_OPT += -DDSO_DLFCN -DHAVE_DLFCN_H -DNDEBUG -DOPENSSL_NO_STATIC_ENGINE -DOPENSSL_PIC -DOPENSSL_IA32_SSE2 -DOPENSSL_BN_ASM_MONT -DOPENSSL_BN_ASM_MONT5 -DOPENSSL_BN_ASM_GF2m -DSHA1_ASM -DSHA256_ASM -DSHA512_ASM -DRC4_ASM -DMD5_ASM -DAES_ASM -DVPAES_ASM -DBSAES_ASM -DGHASH_ASM -DECP_NISTZ256_ASM -DPADLOCK_ASM -DPOLY1305_ASM -DOPENSSL_NO_MD2 -DOPENSSL_NO_RC5 -DOPENSSL_NO_ASAN -DOPENSSL_NO_CRYPTO_MDEBUG -DOPENSSL_NO_CRYPTO_MDEBUG_BACKTRACE -DOPENSSL_NO_EC_NISTP_64_GCC_128 -DOPENSSL_NO_EGD -DOPENSSL_NO_FUZZ_AFL -DOPENSSL_NO_FUZZ_LIBFUZZER -DOPENSSL_NO_HEARTBEATS -DOPENSSL_NO_MSAN -DOPENSSL_NO_SCTP -DOPENSSL_NO_SSL_TRACE -DOPENSSL_NO_SSL3 -DOPENSSL_NO_SSL3_METHOD -DOPENSSL_NO_UBSAN -DOPENSSL_NO_UNIT_TEST -DOPENSSL_NO_WEAK_SSL_CIPHERS

#
# crypto base source
#
#SRC_C = cpt_err.c   cversion.c  ex_data.c  mem_dbg.c  mem_sec.c  o_fips.c   o_init.c  o_time.c        threads_pthread.c  uid.c \
#        cryptlib.c  ebcdic.c    init.c     mem.c      o_dir.c    o_fopen.c  o_str.c   threads_none.c  threads_win.c
SRC_C = cpt_err.c   cversion.c  ex_data.c  mem_clr.c  mem.c      o_dir.c   o_fopen.c  o_str.c   threads_none.c     threads_win.c \
        cryptlib.c  ebcdic.c    init.c     mem_dbg.c  mem_sec.c  o_fips.c  o_init.c   o_time.c  threads_pthread.c  uid.c

SRC_C_aes = \
  aes_cbc.c \
  aes_cfb.c \
  aes_core.c \
  aes_ecb.c \
  aes_ige.c \
  aes_misc.c \
  aes_ofb.c \
  aes_wrap.c
SRC_C_asn1 = \
  a_bitstr.c \
  a_d2i_fp.c \
  a_digest.c \
  a_dup.c \
  a_gentm.c \
  a_i2d_fp.c \
  a_int.c \
  a_mbstr.c \
  ameth_lib.c \
  a_object.c \
  a_octet.c \
  a_print.c \
  a_sign.c \
  asn1_err.c \
  asn1_gen.c \
  asn1_lib.c \
  asn1_par.c \
  asn_mime.c \
  asn_moid.c \
  asn_mstbl.c \
  asn_pack.c \
  a_strex.c \
  a_strnid.c \
  a_time.c \
  a_type.c \
  a_utctm.c \
  a_utf8.c \
  a_verify.c \
  bio_asn1.c \
  bio_ndef.c \
  d2i_pr.c \
  d2i_pu.c \
  evp_asn1.c \
  f_int.c \
  f_string.c \
  i2d_pr.c \
  i2d_pu.c \
  n_pkey.c \
  nsseq.c \
  p5_pbe.c \
  p5_pbev2.c \
  p5_scrypt.c \
  p8_pkey.c \
  tasn_dec.c \
  tasn_enc.c \
  tasn_fre.c \
  tasn_new.c \
  tasn_prn.c \
  tasn_scn.c \
  tasn_typ.c \
  tasn_utl.c \
  t_bitst.c \
  t_pkey.c \
  t_spki.c \
  x_algor.c \
  x_bignum.c \
  x_info.c \
  x_int64.c \
  x_long.c \
  x_pkey.c \
  x_sig.c \
  x_spki.c \
  x_val.c
SRC_C_async = \
  async_err.c \
  async.c \
  async_wait.c
SRC_C_bf = \
  bf_cfb64.c \
  bf_ecb.c \
  bf_enc.c \
  bf_ofb64.c \
  bf_skey.c
SRC_C_bio = \
  b_addr.c \
  b_dump.c \
  bf_buff.c \
  bf_lbuf.c \
  bf_nbio.c \
  bf_null.c \
  bio_cb.c \
  bio_err.c \
  bio_lib.c \
  bio_meth.c \
  b_print.c \
  b_sock2.c \
  b_sock.c \
  bss_acpt.c \
  bss_bio.c \
  bss_conn.c \
  bss_dgram.c \
  bss_fd.c \
  bss_file.c \
  bss_log.c \
  bss_mem.c \
  bss_null.c \
  bss_sock.c
SRC_C_blake2 = \
  blake2b.c \
  blake2s.c \
  m_blake2b.c \
  m_blake2s.c
SRC_C_bn = \
  bn_add.c \
  bn_asm.c \
  bn_blind.c \
  bn_const.c \
  bn_ctx.c \
  bn_depr.c \
  bn_dh.c \
  bn_div.c \
  bn_err.c \
  bn_exp2.c \
  bn_exp.c \
  bn_gcd.c \
  bn_gf2m.c \
  bn_intern.c \
  bn_kron.c \
  bn_lib.c \
  bn_mod.c \
  bn_mont.c \
  bn_mpi.c \
  bn_mul.c \
  bn_nist.c \
  bn_prime.c \
  bn_print.c \
  bn_rand.c \
  bn_recp.c \
  bn_shift.c \
  bn_sqr.c \
  bn_sqrt.c \
  bn_srp.c \
  bn_word.c \
  bn_x931p.c
SRC_C_buffer = \
  buf_err.c \
  buffer.c
SRC_C_camellia = \
  camellia.c \
  cmll_cbc.c \
  cmll_cfb.c \
  cmll_ctr.c \
  cmll_ecb.c \
  cmll_misc.c \
  cmll_ofb.c
SRC_C_cast = \
  c_cfb64.c \
  c_ecb.c \
  c_enc.c \
  c_ofb64.c \
  c_skey.c
SRC_C_chacha = \
  chacha_enc.c
SRC_C_cmac = \
  cmac.c \
  cm_ameth.c \
  cm_pmeth.c
SRC_C_cms = \
  cms_asn1.c \
  cms_att.c \
  cms_cd.c \
  cms_dd.c \
  cms_enc.c \
  cms_env.c \
  cms_err.c \
  cms_ess.c \
  cms_io.c \
  cms_kari.c \
  cms_lib.c \
  cms_pwri.c \
  cms_sd.c \
  cms_smime.c
SRC_C_comp = \
  comp_err.c \
  comp_lib.c \
  c_zlib.c
SRC_C_conf = \
  conf_api.c \
  conf_def.c \
  conf_err.c \
  conf_lib.c \
  conf_mall.c \
  conf_mod.c \
  conf_sap.c \
  conf_ssl.c
SRC_C_ct = \
  ct_b64.c \
  ct_err.c \
  ct_log.c \
  ct_oct.c \
  ct_policy.c \
  ct_prn.c \
  ct_sct_ctx.c \
  ct_sct.c \
  ct_vfy.c \
  ct_x509v3.c
SRC_C_des = \
  cbc_cksm.c \
  cbc_enc.c \
  cfb64ede.c \
  cfb64enc.c \
  cfb_enc.c \
  des_enc.c \
  ecb3_enc.c \
  ecb_enc.c \
  fcrypt_b.c \
  fcrypt.c \
  ofb64ede.c \
  ofb64enc.c \
  ofb_enc.c \
  pcbc_enc.c \
  qud_cksm.c \
  rand_key.c \
  rpc_enc.c \
  set_key.c \
  str2key.c \
  xcbc_enc.c
SRC_C_dh = \
  dh_ameth.c \
  dh_asn1.c \
  dh_check.c \
  dh_depr.c \
  dh_err.c \
  dh_gen.c \
  dh_kdf.c \
  dh_key.c \
  dh_lib.c \
  dh_meth.c \
  dh_pmeth.c \
  dh_prn.c \
  dh_rfc5114.c
SRC_C_dsa = \
  dsa_ameth.c \
  dsa_asn1.c \
  dsa_depr.c \
  dsa_err.c \
  dsa_gen.c \
  dsa_key.c \
  dsa_lib.c \
  dsa_meth.c \
  dsa_ossl.c \
  dsa_pmeth.c \
  dsa_prn.c \
  dsa_sign.c \
  dsa_vrf.c
SRC_C_dso = \
  dso_dlfcn.c \
  dso_dl.c \
  dso_err.c \
  dso_lib.c \
  dso_openssl.c \
  dso_vms.c \
  dso_win32.c
SRC_C_ec = \
  curve25519.c \
  ec2_mult.c \
  ec2_oct.c \
  ec2_smpl.c \
  ec_ameth.c \
  ec_asn1.c \
  ec_check.c \
  ec_curve.c \
  ec_cvt.c \
  ecdh_kdf.c \
  ecdh_ossl.c \
  ecdsa_ossl.c \
  ecdsa_sign.c \
  ecdsa_vrf.c \
  ec_err.c \
  ec_key.c \
  ec_kmeth.c \
  eck_prn.c \
  ec_lib.c \
  ec_mult.c \
  ec_oct.c \
  ec_pmeth.c \
  ecp_mont.c \
  ecp_nist.c \
  ecp_nistp224.c \
  ecp_nistp256.c \
  ecp_nistp521.c \
  ecp_nistputil.c \
  ecp_oct.c \
  ec_print.c \
  ecp_smpl.c \
  ecx_meth.c
SRC_C_engine = \
  eng_all.c \
  eng_cnf.c \
  eng_cryptodev.c \
  eng_ctrl.c \
  eng_dyn.c \
  eng_err.c \
  eng_fat.c \
  eng_init.c \
  eng_lib.c \
  eng_list.c \
  eng_openssl.c \
  eng_pkey.c \
  eng_rdrand.c \
  eng_table.c \
  tb_asnmth.c \
  tb_cipher.c \
  tb_dh.c \
  tb_digest.c \
  tb_dsa.c \
  tb_eckey.c \
  tb_pkmeth.c \
  tb_rand.c \
  tb_rsa.c
SRC_C_err = \
  err_all.c \
  err.c \
  err_prn.c
SRC_C_evp = \
  bio_b64.c \
  bio_enc.c \
  bio_md.c \
  bio_ok.c \
  c_allc.c \
  c_alld.c \
  cmeth_lib.c \
  digest.c \
  e_aes_cbc_hmac_sha1.c \
  e_aes_cbc_hmac_sha256.c \
  e_aes.c \
  e_bf.c \
  e_camellia.c \
  e_cast.c \
  e_chacha20_poly1305.c \
  e_des3.c \
  e_des.c \
  e_idea.c \
  encode.c \
  e_null.c \
  e_old.c \
  e_rc2.c \
  e_rc4_hmac_md5.c \
  e_rc4.c \
  e_rc5.c \
  e_seed.c \
  evp_cnf.c \
  evp_enc.c \
  evp_err.c \
  evp_key.c \
  evp_lib.c \
  evp_pbe.c \
  evp_pkey.c \
  e_xcbc_d.c \
  m_md2.c \
  m_md4.c \
  m_md5.c \
  m_md5_sha1.c \
  m_mdc2.c \
  m_null.c \
  m_ripemd.c \
  m_sha1.c \
  m_sigver.c \
  m_wp.c \
  names.c \
  p5_crpt2.c \
  p5_crpt.c \
  p_dec.c \
  p_enc.c \
  p_lib.c \
  pmeth_fn.c \
  pmeth_gn.c \
  pmeth_lib.c \
  p_open.c \
  p_seal.c \
  p_sign.c \
  p_verify.c \
  scrypt.c
SRC_C_hmac = \
  hmac.c \
  hm_ameth.c \
  hm_pmeth.c
SRC_C_idea = \
  i_cbc.c \
  i_cfb64.c \
  i_ecb.c \
  i_ofb64.c \
  i_skey.c
SRC_C_kdf = \
  hkdf.c \
  kdf_err.c \
  tls1_prf.c
SRC_C_lhash = \
  lhash.c \
  lh_stats.c
SRC_C_md4 = \
  md4_dgst.c \
  md4_one.c
SRC_C_md5 = \
  md5_dgst.c \
  md5_one.c
SRC_C_mdc2 = \
  mdc2dgst.c \
  mdc2_one.c
SRC_C_modes = \
  cbc128.c \
  ccm128.c \
  cfb128.c \
  ctr128.c \
  cts128.c \
  gcm128.c \
  ocb128.c \
  ofb128.c \
  wrap128.c \
  xts128.c
SRC_C_objects = \
  obj_dat.c \
  obj_err.c \
  obj_lib.c \
  obj_xref.c \
  o_names.c
SRC_C_ocsp = \
  ocsp_asn.c \
  ocsp_cl.c \
  ocsp_err.c \
  ocsp_ext.c \
  ocsp_ht.c \
  ocsp_lib.c \
  ocsp_prn.c \
  ocsp_srv.c \
  ocsp_vfy.c \
  v3_ocsp.c
SRC_C_pem = \
  pem_all.c \
  pem_err.c \
  pem_info.c \
  pem_lib.c \
  pem_oth.c \
  pem_pk8.c \
  pem_pkey.c \
  pem_sign.c \
  pem_x509.c \
  pem_xaux.c \
  pvkfmt.c
SRC_C_pkcs12 = \
  p12_add.c \
  p12_asn.c \
  p12_attr.c \
  p12_crpt.c \
  p12_crt.c \
  p12_decr.c \
  p12_init.c \
  p12_key.c \
  p12_kiss.c \
  p12_mutl.c \
  p12_npas.c \
  p12_p8d.c \
  p12_p8e.c \
  p12_sbag.c \
  p12_utl.c \
  pk12err.c
SRC_C_pkcs7 = \
  bio_pk7.c \
  pk7_asn1.c \
  pk7_attr.c \
  pk7_doit.c \
  pk7_lib.c \
  pk7_mime.c \
  pk7_smime.c \
  pkcs7err.c
SRC_C_poly1305 = \
  poly1305.c
SRC_C_rand = \
  md_rand.c \
  rand_egd.c \
  rand_err.c \
  randfile.c \
  rand_lib.c \
  rand_unix.c \
  rand_vms.c \
  rand_win.c
SRC_C_rc2 = \
  rc2_cbc.c \
  rc2cfb64.c \
  rc2_ecb.c \
  rc2ofb64.c \
  rc2_skey.c
SRC_C_rc4 = \
  rc4_enc.c \
  rc4_skey.c
SRC_C_ripemd = \
  rmd_dgst.c \
  rmd_one.c
SRC_C_rsa = \
  rsa_ameth.c \
  rsa_asn1.c \
  rsa_chk.c \
  rsa_crpt.c \
  rsa_depr.c \
  rsa_err.c \
  rsa_gen.c \
  rsa_lib.c \
  rsa_meth.c \
  rsa_none.c \
  rsa_null.c \
  rsa_oaep.c \
  rsa_ossl.c \
  rsa_pk1.c \
  rsa_pmeth.c \
  rsa_prn.c \
  rsa_pss.c \
  rsa_saos.c \
  rsa_sign.c \
  rsa_ssl.c \
  rsa_x931g.c \
  rsa_x931.c
SRC_C_seed = \
  seed_cbc.c \
  seed_cfb.c \
  seed_ecb.c \
  seed.c \
  seed_ofb.c
SRC_C_sha = \
  sha1dgst.c \
  sha1_one.c \
  sha256.c \
  sha512.c
SRC_C_srp = \
  srp_lib.c \
  srp_vfy.c
SRC_C_stack = \
  stack.c
SRC_C_ts = \
  ts_asn1.c \
  ts_conf.c \
  ts_err.c \
  ts_lib.c \
  ts_req_print.c \
  ts_req_utils.c \
  ts_rsp_print.c \
  ts_rsp_sign.c \
  ts_rsp_utils.c \
  ts_rsp_verify.c \
  ts_verify_ctx.c
SRC_C_txt_db = \
  txt_db.c
SRC_C_ui = \
  ui_err.c \
  ui_lib.c \
  ui_openssl.c \
  ui_util.c
SRC_C_whrlpool = \
  wp_block.c \
  wp_dgst.c
SRC_C_x509 = \
  by_dir.c \
  by_file.c \
  t_crl.c \
  t_req.c \
  t_x509.c \
  x509_att.c \
  x509_cmp.c \
  x509cset.c \
  x509_d2.c \
  x509_def.c \
  x509_err.c \
  x509_ext.c \
  x509_lu.c \
  x509_meth.c \
  x509name.c \
  x509_obj.c \
  x509_r2x.c \
  x509_req.c \
  x509rset.c \
  x509_set.c \
  x509spki.c \
  x509_trs.c \
  x509_txt.c \
  x509type.c \
  x509_v3.c \
  x509_vfy.c \
  x509_vpm.c \
  x_all.c \
  x_attrib.c \
  x_crl.c \
  x_exten.c \
  x_name.c \
  x_pubkey.c \
  x_req.c \
  x_x509a.c \
  x_x509.c
SRC_C_x509v3 = \
  pcy_cache.c \
  pcy_data.c \
  pcy_lib.c \
  pcy_map.c \
  pcy_node.c \
  pcy_tree.c \
  v3_addr.c \
  v3_akeya.c \
  v3_akey.c \
  v3_alt.c \
  v3_asid.c \
  v3_bcons.c \
  v3_bitst.c \
  v3_conf.c \
  v3_cpols.c \
  v3_crld.c \
  v3_enum.c \
  v3err.c \
  v3_extku.c \
  v3_genn.c \
  v3_ia5.c \
  v3_info.c \
  v3_int.c \
  v3_lib.c \
  v3_ncons.c \
  v3_pcia.c \
  v3_pci.c \
  v3_pcons.c \
  v3_pku.c \
  v3_pmaps.c \
  v3_prn.c \
  v3_purp.c \
  v3_skey.c \
  v3_sxnet.c \
  v3_tlsf.c \
  v3_utl.c


SRC_DIR = aes \
          asn1 \
          async \
          bf \
          bio \
          blake2 \
          bn \
          buffer \
          camellia \
          cast \
          chacha \
          cmac \
          cms \
          comp \
          conf \
          ct \
          des \
          dh \
          dsa \
          dso \
          ec \
          engine \
          err \
          evp \
          hmac \
          idea \
          kdf \
          lhash \
          md4 \
          md5 \
          mdc2 \
          modes \
          objects \
          ocsp \
          pem \
          pkcs12 \
          pkcs7 \
          poly1305 \
          rand \
          rc2 \
          rc4 \
          ripemd \
          rsa \
          seed \
          sha \
          srp \
          stack \
          ts \
          txt_db \
          ui \
          whrlpool \
          x509 \
          x509v3

SRC_C += $(foreach dir,$(SRC_DIR),$(addprefix $(dir)/,$(SRC_C_$(dir))))

INC_DIR += $(LIBCRYPTO_DIR)/asn1
INC_DIR += $(LIBCRYPTO_DIR)/evp
INC_DIR += $(LIBCRYPTO_DIR)/modes
INC_DIR += $(LIBCRYPTO_DIR)
INC_DIR += $(LIBCRYPTO_DIR)/../
INC_DIR += $(OPENSSL_PORT_DIR)/include
INC_DIR += $(LIBCRYPTO_DIR)/modes
INC_DIR += $(LIBCRYPTO_DIR)
INC_DIR += $(LIBCRYPTO_DIR)/../
INC_DIR += /home/esandberg/projects/embedded/genode-18.08/genode-18.08/contrib/openssl-d4ff46a8d3b1eb0eaddc635718e99d088330c18c/src/lib/openssl/include/
INC_DIR += /home/esandberg/projects/embedded/genode-18.08/genode-18.08/contrib/openssl-d4ff46a8d3b1eb0eaddc635718e99d088330c18c/src/lib/openssl/crypto/include

#
# Generate buildinf.h
#
$(SRC_C:.c=.o): buildinf.h

# Rules taken from FreeBSDs libcrypto/Makefile
buildinf.h:
	$(VERBOSE)( echo "#ifndef MK1MF_BUILD"; \
	echo "  /* auto-generated by libcrypto.mk for crypto/cversion.c */"; \
	echo "  #define CFLAGS \"$(CC)\""; \
	echo "  #define PLATFORM \"FreeBSD-$(TARGET_CPUARCH)\""; \
	echo "#endif" ) > $@

vpath %.c $(LIBCRYPTO_DIR)
