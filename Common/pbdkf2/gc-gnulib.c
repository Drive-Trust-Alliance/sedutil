/* gc-gnulib.c --- Common gnulib internal crypto interface functions
 * Copyright (C) 2002-2015 Free Software Foundation, Inc.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; if not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifdef _MSC_VER
#pragma warning(push,3)
#endif
/* Note: This file is only built if GC uses internal functions. */

#include "config.h"

/* Get prototype. */
#include "gc.h"

#include <stdlib.h>
#include <string.h>

/* For randomize. */
#ifdef GNULIB_GC_RANDOM
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <errno.h>
#endif

/* Hashes. */
#ifdef GNULIB_GC_MD2
# include "md2.h"
#endif
#ifdef GNULIB_GC_MD4
# include "md4.h"
#endif
#ifdef GNULIB_GC_MD5
# include "md5.h"
#endif
#ifdef GNULIB_GC_SHA1
# include "sha1.h"
#endif
#if defined(GNULIB_GC_HMAC_MD5) || defined(GNULIB_GC_HMAC_SHA1) || defined(GNULIB_GC_HMAC_SHA256) || defined(GNULIB_GC_HMAC_SHA512)
# include "hmac.h"
#endif

/* Ciphers. */
#ifdef GNULIB_GC_ARCFOUR
# include "arcfour.h"
#endif
#ifdef GNULIB_GC_ARCTWO
# include "arctwo.h"
#endif
#ifdef GNULIB_GC_DES
# include "des.h"
#endif
#ifdef GNULIB_GC_RIJNDAEL
# include "rijndael-api-fst.h"
#endif

#ifdef GNULIB_GC_RANDOM
# if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
#  include <windows.h>
#  include <wincrypt.h>
HCRYPTPROV g_hProv = 0;
#  ifndef PROV_INTEL_SEC
#   define PROV_INTEL_SEC 22
#  endif
#  ifndef CRYPT_VERIFY_CONTEXT
#   define CRYPT_VERIFY_CONTEXT 0xF0000000
#  endif
# endif
#endif

Gc_rc
gc_init (void)
{
#ifdef GNULIB_GC_RANDOM
# if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
  if (g_hProv)
    CryptReleaseContext (g_hProv, 0);

  /* There is no need to create a container for just random data, so
     we can use CRYPT_VERIFY_CONTEXT (one call) see:
     http://blogs.msdn.com/dangriff/archive/2003/11/19/51709.aspx */

  /* We first try to use the Intel PIII RNG if drivers are present */
  if (!CryptAcquireContext (&g_hProv, NULL, NULL,
                            PROV_INTEL_SEC, CRYPT_VERIFY_CONTEXT))
    {
      /* not a PIII or no drivers available, use default RSA CSP */
      if (!CryptAcquireContext (&g_hProv, NULL, NULL,
                                PROV_RSA_FULL, CRYPT_VERIFY_CONTEXT))
        return GC_RANDOM_ERROR;
    }
# endif
#endif

  return GC_OK;
}

void
gc_done (void)
{
#ifdef GNULIB_GC_RANDOM
# if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
  if (g_hProv)
    {
      CryptReleaseContext (g_hProv, 0);
      g_hProv = 0;
    }
# endif
#endif

  return;
}

#ifdef GNULIB_GC_RANDOM

/* Randomness. */

static Gc_rc
randomize (int level, char *data, size_t datalen)
{
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
  if (!g_hProv)
    return GC_RANDOM_ERROR;
  CryptGenRandom (g_hProv, (DWORD) datalen, data);
#else
  int fd;
  const char *device;
  size_t len = 0;
  int rc;

  switch (level)
    {
    case 0:
      device = NAME_OF_NONCE_DEVICE;
      break;

    case 1:
      device = NAME_OF_PSEUDO_RANDOM_DEVICE;
      break;

    default:
      device = NAME_OF_RANDOM_DEVICE;
      break;
    }

  if (strcmp (device, "no") == 0)
    return GC_RANDOM_ERROR;

  fd = open (device, O_RDONLY);
  if (fd < 0)
    return GC_RANDOM_ERROR;

  do
    {
      ssize_t tmp;

      tmp = read (fd, data, datalen);

      if (tmp < 0)
        {
          int save_errno = errno;
          close (fd);
          errno = save_errno;
          return GC_RANDOM_ERROR;
        }

      len += tmp;
    }
  while (len < datalen);

  rc = close (fd);
  if (rc < 0)
    return GC_RANDOM_ERROR;
#endif

  return GC_OK;
}

Gc_rc
gc_nonce (char *data, size_t datalen)
{
  return randomize (0, data, datalen);
}

Gc_rc
gc_pseudo_random (char *data, size_t datalen)
{
  return randomize (1, data, datalen);
}

Gc_rc
gc_random (char *data, size_t datalen)
{
  return randomize (2, data, datalen);
}

#endif

/* Memory allocation. */

void
gc_set_allocators (gc_malloc_t func_malloc,
                   gc_malloc_t secure_malloc,
                   gc_secure_check_t secure_check,
                   gc_realloc_t func_realloc, gc_free_t func_free)
{
  return;
}

/* Ciphers. */

typedef struct _gc_cipher_ctx
{
  Gc_cipher alg;
  Gc_cipher_mode mode;
#ifdef GNULIB_GC_ARCTWO
  arctwo_context arctwoContext;
  char arctwoIV[ARCTWO_BLOCK_SIZE];
#endif
#ifdef GNULIB_GC_ARCFOUR
  arcfour_context arcfourContext;
#endif
#ifdef GNULIB_GC_DES
  gl_des_ctx desContext;
#endif
#ifdef GNULIB_GC_RIJNDAEL
  rijndaelKeyInstance aesEncKey;
  rijndaelKeyInstance aesDecKey;
  rijndaelCipherInstance aesContext;
#endif
} _gc_cipher_ctx;

Gc_rc
gc_cipher_open (Gc_cipher alg, Gc_cipher_mode mode,
                gc_cipher_handle * outhandle)
{
  _gc_cipher_ctx *ctx;
  Gc_rc rc = GC_OK;

  ctx = calloc (sizeof (*ctx), 1);
  if (!ctx)
    return GC_MALLOC_ERROR;

  ctx->alg = alg;
  ctx->mode = mode;

  switch (alg)
    {
#ifdef GNULIB_GC_ARCTWO
    case GC_ARCTWO40:
      switch (mode)
        {
        case GC_ECB:
        case GC_CBC:
          break;

        default:
          rc = GC_INVALID_CIPHER;
        }
      break;
#endif

#ifdef GNULIB_GC_ARCFOUR
    case GC_ARCFOUR128:
    case GC_ARCFOUR40:
      switch (mode)
        {
        case GC_STREAM:
          break;

        default:
          rc = GC_INVALID_CIPHER;
        }
      break;
#endif

#ifdef GNULIB_GC_DES
    case GC_DES:
      switch (mode)
        {
        case GC_ECB:
          break;

        default:
          rc = GC_INVALID_CIPHER;
        }
      break;
#endif

#ifdef GNULIB_GC_RIJNDAEL
    case GC_AES128:
    case GC_AES192:
    case GC_AES256:
      switch (mode)
        {
        case GC_ECB:
        case GC_CBC:
          break;

        default:
          rc = GC_INVALID_CIPHER;
        }
      break;
#endif

    default:
      rc = GC_INVALID_CIPHER;
    }

  if (rc == GC_OK)
    *outhandle = ctx;
  else
    free (ctx);

  return rc;
}
#ifdef _MSC_VER
#pragma warning(disable:4702)
#endif
Gc_rc
gc_cipher_setkey (gc_cipher_handle handle, size_t keylen, const char *key)
{
  _gc_cipher_ctx *ctx = handle;

  switch (ctx->alg)
    {
#ifdef GNULIB_GC_ARCTWO
    case GC_ARCTWO40:
      arctwo_setkey (&ctx->arctwoContext, keylen, key);
      break;
#endif

#ifdef GNULIB_GC_ARCFOUR
    case GC_ARCFOUR128:
    case GC_ARCFOUR40:
      arcfour_setkey (&ctx->arcfourContext, key, keylen);
      break;
#endif

#ifdef GNULIB_GC_DES
    case GC_DES:
      if (keylen != 8)
        return GC_INVALID_CIPHER;
      gl_des_setkey (&ctx->desContext, key);
      break;
#endif

#ifdef GNULIB_GC_RIJNDAEL
    case GC_AES128:
    case GC_AES192:
    case GC_AES256:
      {
        rijndael_rc rc;
        size_t i;
        char keyMaterial[RIJNDAEL_MAX_KEY_SIZE + 1];

        for (i = 0; i < keylen; i++)
          sprintf (&keyMaterial[2 * i], "%02x", key[i] & 0xFF);

        rc = rijndaelMakeKey (&ctx->aesEncKey, RIJNDAEL_DIR_ENCRYPT,
                              keylen * 8, keyMaterial);
        if (rc < 0)
          return GC_INVALID_CIPHER;

        rc = rijndaelMakeKey (&ctx->aesDecKey, RIJNDAEL_DIR_DECRYPT,
                              keylen * 8, keyMaterial);
        if (rc < 0)
          return GC_INVALID_CIPHER;

        rc = rijndaelCipherInit (&ctx->aesContext, RIJNDAEL_MODE_ECB, NULL);
        if (rc < 0)
          return GC_INVALID_CIPHER;
      }
      break;
#endif

    default:
      return GC_INVALID_CIPHER;
    }
#ifdef _MSC_VER
#pragma warning(default:4702)
#endif

  return GC_OK;
}
#ifdef _MSC_VER
#pragma warning(disable:4702)
#endif

Gc_rc
gc_cipher_setiv (gc_cipher_handle handle, size_t ivlen, const char *iv)
{
  _gc_cipher_ctx *ctx = handle;

  switch (ctx->alg)
    {
#ifdef GNULIB_GC_ARCTWO
    case GC_ARCTWO40:
      if (ivlen != ARCTWO_BLOCK_SIZE)
        return GC_INVALID_CIPHER;
      memcpy (ctx->arctwoIV, iv, ivlen);
      break;
#endif

#ifdef GNULIB_GC_RIJNDAEL
    case GC_AES128:
    case GC_AES192:
    case GC_AES256:
      switch (ctx->mode)
        {
        case GC_ECB:
          /* Doesn't use IV. */
          break;

        case GC_CBC:
          {
            rijndael_rc rc;
            size_t i;
            char ivMaterial[2 * RIJNDAEL_MAX_IV_SIZE + 1];

            for (i = 0; i < ivlen; i++)
              sprintf (&ivMaterial[2 * i], "%02x", iv[i] & 0xFF);

            rc = rijndaelCipherInit (&ctx->aesContext, RIJNDAEL_MODE_CBC,
                                     ivMaterial);
            if (rc < 0)
              return GC_INVALID_CIPHER;
          }
          break;

        default:
          return GC_INVALID_CIPHER;
        }
      break;
#endif

    default:
      return GC_INVALID_CIPHER;
    }

  return GC_OK;
}
#ifdef _MSC_VER
#pragma warning(default:4702)
#endif
#ifdef _MSC_VER
#pragma warning(disable:4702)
#endif

Gc_rc
gc_cipher_encrypt_inline (gc_cipher_handle handle, size_t len, char *data)
{
  _gc_cipher_ctx *ctx = handle;

  switch (ctx->alg)
    {
#ifdef GNULIB_GC_ARCTWO
    case GC_ARCTWO40:
      switch (ctx->mode)
        {
        case GC_ECB:
          arctwo_encrypt (&ctx->arctwoContext, data, data, len);
          break;

        case GC_CBC:
          for (; len >= ARCTWO_BLOCK_SIZE; len -= ARCTWO_BLOCK_SIZE,
               data += ARCTWO_BLOCK_SIZE)
            {
              size_t i;
              for (i = 0; i < ARCTWO_BLOCK_SIZE; i++)
                data[i] ^= ctx->arctwoIV[i];
              arctwo_encrypt (&ctx->arctwoContext, data, data,
                              ARCTWO_BLOCK_SIZE);
              memcpy (ctx->arctwoIV, data, ARCTWO_BLOCK_SIZE);
            }
          break;

        default:
          return GC_INVALID_CIPHER;
        }
      break;
#endif

#ifdef GNULIB_GC_ARCFOUR
    case GC_ARCFOUR128:
    case GC_ARCFOUR40:
      arcfour_stream (&ctx->arcfourContext, data, data, len);
      break;
#endif

#ifdef GNULIB_GC_DES
    case GC_DES:
      for (; len >= 8; len -= 8, data += 8)
        gl_des_ecb_encrypt (&ctx->desContext, data, data);
      break;
#endif

#ifdef GNULIB_GC_RIJNDAEL
    case GC_AES128:
    case GC_AES192:
    case GC_AES256:
      {
        int nblocks;

        nblocks = rijndaelBlockEncrypt (&ctx->aesContext, &ctx->aesEncKey,
                                        data, 8 * len, data);
        if (nblocks < 0)
          return GC_INVALID_CIPHER;
      }
      break;
#endif

    default:
      return GC_INVALID_CIPHER;
    }

  return GC_OK;
}

Gc_rc
gc_cipher_decrypt_inline (gc_cipher_handle handle, size_t len, char *data)
{
  _gc_cipher_ctx *ctx = handle;

  switch (ctx->alg)
    {
#ifdef GNULIB_GC_ARCTWO
    case GC_ARCTWO40:
      switch (ctx->mode)
        {
        case GC_ECB:
          arctwo_decrypt (&ctx->arctwoContext, data, data, len);
          break;

        case GC_CBC:
          for (; len >= ARCTWO_BLOCK_SIZE; len -= ARCTWO_BLOCK_SIZE,
               data += ARCTWO_BLOCK_SIZE)
            {
              char tmpIV[ARCTWO_BLOCK_SIZE];
              size_t i;
              memcpy (tmpIV, data, ARCTWO_BLOCK_SIZE);
              arctwo_decrypt (&ctx->arctwoContext, data, data,
                              ARCTWO_BLOCK_SIZE);
              for (i = 0; i < ARCTWO_BLOCK_SIZE; i++)
                data[i] ^= ctx->arctwoIV[i];
              memcpy (ctx->arctwoIV, tmpIV, ARCTWO_BLOCK_SIZE);
            }
          break;

        default:
          return GC_INVALID_CIPHER;
        }
      break;
#endif

#ifdef GNULIB_GC_ARCFOUR
    case GC_ARCFOUR128:
    case GC_ARCFOUR40:
      arcfour_stream (&ctx->arcfourContext, data, data, len);
      break;
#endif

#ifdef GNULIB_GC_DES
    case GC_DES:
      for (; len >= 8; len -= 8, data += 8)
        gl_des_ecb_decrypt (&ctx->desContext, data, data);
      break;
#endif

#ifdef GNULIB_GC_RIJNDAEL
    case GC_AES128:
    case GC_AES192:
    case GC_AES256:
      {
        int nblocks;

        nblocks = rijndaelBlockDecrypt (&ctx->aesContext, &ctx->aesDecKey,
                                        data, 8 * len, data);
        if (nblocks < 0)
          return GC_INVALID_CIPHER;
      }
      break;
#endif

    default:
      return GC_INVALID_CIPHER;
    }

  return GC_OK;
}
#ifdef _MSC_VER
#pragma warning(default:4702)
#endif

Gc_rc
gc_cipher_close (gc_cipher_handle handle)
{
  _gc_cipher_ctx *ctx = handle;

  free (ctx);

  return GC_OK;
}

/* Hashes. */

#define MAX_DIGEST_SIZE 20

typedef struct _gc_hash_ctx
{
  Gc_hash alg;
  Gc_hash_mode mode;
  char hash[MAX_DIGEST_SIZE];
#ifdef GNULIB_GC_MD2
  struct md2_ctx md2Context;
#endif
#ifdef GNULIB_GC_MD4
  struct md4_ctx md4Context;
#endif
#ifdef GNULIB_GC_MD5
  struct md5_ctx md5Context;
#endif
#ifdef GNULIB_GC_SHA1
  struct sha1_ctx sha1Context;
#endif
} _gc_hash_ctx;

Gc_rc
gc_hash_open (Gc_hash hash, Gc_hash_mode mode, gc_hash_handle * outhandle)
{
  _gc_hash_ctx *ctx;
  Gc_rc rc = GC_OK;

  ctx = calloc (sizeof (*ctx), 1);
  if (!ctx)
    return GC_MALLOC_ERROR;

  ctx->alg = hash;
  ctx->mode = mode;

  switch (hash)
    {
#ifdef GNULIB_GC_MD2
    case GC_MD2:
      md2_init_ctx (&ctx->md2Context);
      break;
#endif

#ifdef GNULIB_GC_MD4
    case GC_MD4:
      md4_init_ctx (&ctx->md4Context);
      break;
#endif

#ifdef GNULIB_GC_MD5
    case GC_MD5:
      md5_init_ctx (&ctx->md5Context);
      break;
#endif

#ifdef GNULIB_GC_SHA1
    case GC_SHA1:
      sha1_init_ctx (&ctx->sha1Context);
      break;
#endif

    default:
      rc = GC_INVALID_HASH;
      break;
    }

  switch (mode)
    {
    case 0:
      break;

    default:
      rc = GC_INVALID_HASH;
      break;
    }

  if (rc == GC_OK)
    *outhandle = ctx;
  else
    free (ctx);

  return rc;
}

Gc_rc
gc_hash_clone (gc_hash_handle handle, gc_hash_handle * outhandle)
{
  _gc_hash_ctx *in = handle;
  _gc_hash_ctx *out;

  *outhandle = out = calloc (sizeof (*out), 1);
  if (!out)
    return GC_MALLOC_ERROR;

  memcpy (out, in, sizeof (*out));

  return GC_OK;
}

size_t
gc_hash_digest_length (Gc_hash hash)
{
  size_t len;

  switch (hash)
    {
    case GC_MD2:
      len = GC_MD2_DIGEST_SIZE;
      break;

    case GC_MD4:
      len = GC_MD4_DIGEST_SIZE;
      break;

    case GC_MD5:
      len = GC_MD5_DIGEST_SIZE;
      break;

    case GC_RMD160:
      len = GC_RMD160_DIGEST_SIZE;
      break;

    case GC_SHA1:
      len = GC_SHA1_DIGEST_SIZE;
      break;

    default:
      return 0;
    }

  return len;
}

void
gc_hash_write (gc_hash_handle handle, size_t len, const char *data)
{
  _gc_hash_ctx *ctx = handle;

  switch (ctx->alg)
    {
#ifdef GNULIB_GC_MD2
    case GC_MD2:
      md2_process_bytes (data, len, &ctx->md2Context);
      break;
#endif

#ifdef GNULIB_GC_MD4
    case GC_MD4:
      md4_process_bytes (data, len, &ctx->md4Context);
      break;
#endif

#ifdef GNULIB_GC_MD5
    case GC_MD5:
      md5_process_bytes (data, len, &ctx->md5Context);
      break;
#endif

#ifdef GNULIB_GC_SHA1
    case GC_SHA1:
      sha1_process_bytes (data, len, &ctx->sha1Context);
      break;
#endif

    default:
      break;
    }
}

const char *
gc_hash_read (gc_hash_handle handle)
{
  _gc_hash_ctx *ctx = handle;
  const char *ret = NULL;

  switch (ctx->alg)
    {
#ifdef GNULIB_GC_MD2
    case GC_MD2:
      md2_finish_ctx (&ctx->md2Context, ctx->hash);
      ret = ctx->hash;
      break;
#endif

#ifdef GNULIB_GC_MD4
    case GC_MD4:
      md4_finish_ctx (&ctx->md4Context, ctx->hash);
      ret = ctx->hash;
      break;
#endif

#ifdef GNULIB_GC_MD5
    case GC_MD5:
      md5_finish_ctx (&ctx->md5Context, ctx->hash);
      ret = ctx->hash;
      break;
#endif

#ifdef GNULIB_GC_SHA1
    case GC_SHA1:
      sha1_finish_ctx (&ctx->sha1Context, ctx->hash);
      ret = ctx->hash;
      break;
#endif

    default:
      return NULL;
    }

  return ret;
}

void
gc_hash_close (gc_hash_handle handle)
{
  _gc_hash_ctx *ctx = handle;

  free (ctx);
}

Gc_rc
gc_hash_buffer (Gc_hash hash, const void *in, size_t inlen, char *resbuf)
{
  switch (hash)
    {
#ifdef GNULIB_GC_MD2
    case GC_MD2:
      md2_buffer (in, inlen, resbuf);
      break;
#endif

#ifdef GNULIB_GC_MD4
    case GC_MD4:
      md4_buffer (in, inlen, resbuf);
      break;
#endif

#ifdef GNULIB_GC_MD5
    case GC_MD5:
      md5_buffer (in, inlen, resbuf);
      break;
#endif

#ifdef GNULIB_GC_SHA1
    case GC_SHA1:
      sha1_buffer (in, inlen, resbuf);
      break;
#endif

    default:
      return GC_INVALID_HASH;
    }

  return GC_OK;
}

#ifdef GNULIB_GC_MD2
Gc_rc
gc_md2 (const void *in, size_t inlen, void *resbuf)
{
  md2_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#ifdef GNULIB_GC_MD4
Gc_rc
gc_md4 (const void *in, size_t inlen, void *resbuf)
{
  md4_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#ifdef GNULIB_GC_MD5
Gc_rc
gc_md5 (const void *in, size_t inlen, void *resbuf)
{
  md5_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#ifdef GNULIB_GC_SHA1
Gc_rc
gc_sha1 (const void *in, size_t inlen, void *resbuf)
{
  sha1_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#ifdef GNULIB_GC_HMAC_MD5
Gc_rc
gc_hmac_md5 (const void *key, size_t keylen,
             const void *in, size_t inlen, char *resbuf)
{
  hmac_md5 (key, keylen, in, inlen, resbuf);
  return GC_OK;
}
#endif

#ifdef GNULIB_GC_HMAC_SHA1
Gc_rc
gc_hmac_sha1 (const void *key, size_t keylen,
              const void *in, size_t inlen, char *resbuf)
{
  hmac_sha1 (key, keylen, in, inlen, resbuf);
  return GC_OK;
}
#endif

#ifdef GNULIB_GC_HMAC_SHA256
Gc_rc
gc_hmac_sha256 (const void *key, size_t keylen,
                const void *in, size_t inlen, char *resbuf)
{
  hmac_sha256 (key, keylen, in, inlen, resbuf);
  return GC_OK;
}
#endif

#ifdef GNULIB_GC_HMAC_SHA512
Gc_rc
gc_hmac_sha512 (const void *key, size_t keylen,
                const void *in, size_t inlen, char *resbuf)
{
  hmac_sha512 (key, keylen, in, inlen, resbuf);
  return GC_OK;
}
#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif

