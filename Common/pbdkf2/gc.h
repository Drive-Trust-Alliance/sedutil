/* gc.h --- Header file for implementation agnostic crypto wrapper API.
 * Copyright (C) 2002-2005, 2007-2008, 2011-2015 Free Software Foundation, Inc.
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

#ifndef GC_H
# define GC_H


/* Get size_t. */
# include <stddef.h>

enum Gc_rc
{
  GC_OK = 0,
  GC_MALLOC_ERROR,
  GC_INIT_ERROR,
  GC_RANDOM_ERROR,
  GC_INVALID_CIPHER,
  GC_INVALID_HASH,
  GC_PKCS5_INVALID_ITERATION_COUNT,
  GC_PKCS5_INVALID_DERIVED_KEY_LENGTH,
  GC_PKCS5_DERIVED_KEY_TOO_LONG
};
typedef enum Gc_rc Gc_rc;

/* Hash types. */
enum Gc_hash
{
  GC_MD4,
  GC_MD5,
  GC_SHA1,
  GC_MD2,
  GC_RMD160,
  GC_SHA256,
  GC_SHA384,
  GC_SHA512,
  GC_SHA224
};
typedef enum Gc_hash Gc_hash;

enum Gc_hash_mode
{
  GC_HMAC = 1
};
typedef enum Gc_hash_mode Gc_hash_mode;

typedef void *gc_hash_handle;

#define GC_MD2_DIGEST_SIZE 16
#define GC_MD4_DIGEST_SIZE 16
#define GC_MD5_DIGEST_SIZE 16
#define GC_RMD160_DIGEST_SIZE 20
#define GC_SHA1_DIGEST_SIZE 20
#define GC_SHA256_DIGEST_SIZE 32
#define GC_SHA384_DIGEST_SIZE 48
#define GC_SHA512_DIGEST_SIZE 64
#define GC_SHA224_DIGEST_SIZE 24

/* Cipher types. */
enum Gc_cipher
{
  GC_AES128,
  GC_AES192,
  GC_AES256,
  GC_3DES,
  GC_DES,
  GC_ARCFOUR128,
  GC_ARCFOUR40,
  GC_ARCTWO40,
  GC_CAMELLIA128,
  GC_CAMELLIA256
};
typedef enum Gc_cipher Gc_cipher;

enum Gc_cipher_mode
{
  GC_ECB,
  GC_CBC,
  GC_STREAM
};
typedef enum Gc_cipher_mode Gc_cipher_mode;

typedef void *gc_cipher_handle;

/* Call before respectively after any other functions. */
extern Gc_rc gc_init (void);
extern void gc_done (void);

/* Memory allocation (avoid). */
typedef void *(*gc_malloc_t) (size_t n);
typedef int (*gc_secure_check_t) (const void *);
typedef void *(*gc_realloc_t) (void *p, size_t n);
typedef void (*gc_free_t) (void *);
extern void gc_set_allocators (gc_malloc_t func_malloc,
                               gc_malloc_t secure_malloc,
                               gc_secure_check_t secure_check,
                               gc_realloc_t func_realloc,
                               gc_free_t func_free);

/* Randomness. */
extern Gc_rc gc_nonce (char *data, size_t datalen);
extern Gc_rc gc_pseudo_random (char *data, size_t datalen);
extern Gc_rc gc_random (char *data, size_t datalen);

/* Ciphers. */
extern Gc_rc gc_cipher_open (Gc_cipher cipher, Gc_cipher_mode mode,
                             gc_cipher_handle *outhandle);
extern Gc_rc gc_cipher_setkey (gc_cipher_handle handle,
                               size_t keylen, const char *key);
extern Gc_rc gc_cipher_setiv (gc_cipher_handle handle,
                              size_t ivlen, const char *iv);
extern Gc_rc gc_cipher_encrypt_inline (gc_cipher_handle handle,
                                       size_t len, char *data);
extern Gc_rc gc_cipher_decrypt_inline (gc_cipher_handle handle,
                                       size_t len, char *data);
extern Gc_rc gc_cipher_close (gc_cipher_handle handle);

/* Hashes. */

extern Gc_rc gc_hash_open (Gc_hash hash, Gc_hash_mode mode,
                           gc_hash_handle *outhandle);
extern Gc_rc gc_hash_clone (gc_hash_handle handle, gc_hash_handle *outhandle);
extern size_t gc_hash_digest_length (Gc_hash hash);
extern void gc_hash_hmac_setkey (gc_hash_handle handle,
                                 size_t len, const char *key);
extern void gc_hash_write (gc_hash_handle handle,
                           size_t len, const char *data);
extern const char *gc_hash_read (gc_hash_handle handle);
extern void gc_hash_close (gc_hash_handle handle);

/* Compute a hash value over buffer IN of INLEN bytes size using the
   algorithm HASH, placing the result in the pre-allocated buffer OUT.
   The required size of OUT depends on HASH, and is generally
   GC_<HASH>_DIGEST_SIZE.  For example, for GC_MD5 the output buffer
   must be 16 bytes.  The return value is 0 (GC_OK) on success, or
   another Gc_rc error code. */
extern Gc_rc
gc_hash_buffer (Gc_hash hash, const void *in, size_t inlen, char *out);

/* One-call interface. */
extern Gc_rc gc_md2 (const void *in, size_t inlen, void *resbuf);
extern Gc_rc gc_md4 (const void *in, size_t inlen, void *resbuf);
extern Gc_rc gc_md5 (const void *in, size_t inlen, void *resbuf);
extern Gc_rc gc_sha1 (const void *in, size_t inlen, void *resbuf);
extern Gc_rc gc_hmac_md5 (const void *key, size_t keylen,
                          const void *in, size_t inlen, char *resbuf);
extern Gc_rc gc_hmac_sha1 (const void *key, size_t keylen,
                           const void *in, size_t inlen, char *resbuf);
extern Gc_rc gc_hmac_sha256 (const void *key, size_t keylen,
                             const void *in, size_t inlen, char *resbuf);
extern Gc_rc gc_hmac_sha512 (const void *key, size_t keylen,
                             const void *in, size_t inlen, char *resbuf);

/* Derive cryptographic keys from a password P of length PLEN, with
   salt S of length SLEN, placing the result in pre-allocated buffer
   DK of length DKLEN.  An iteration count is specified in C, where a
   larger value means this function take more time (typical iteration
   counts are 1000-20000).  This function "stretches" the key to be
   exactly dkLen bytes long.  GC_OK is returned on success, otherwise
   a Gc_rc error code is returned.  */
extern Gc_rc
gc_pbkdf2_sha1 (const char *P, size_t Plen,
                const char *S, size_t Slen,
                unsigned int c, char *DK, size_t dkLen);

/*
  TODO:

  From: Simon Josefsson <jas@extundo.com>
  Subject: Re: generic crypto
  Newsgroups: gmane.comp.lib.gnulib.bugs
  Cc: bug-gnulib@gnu.org
  Date: Fri, 07 Oct 2005 12:50:57 +0200
  Mail-Copies-To: nobody

  Paul Eggert <eggert@CS.UCLA.EDU> writes:

  > Simon Josefsson <jas@extundo.com> writes:
  >
  >> * Perhaps the /dev/?random reading should be separated into a separate
  >>   module?  It might be useful outside of the gc layer too.
  >
  > Absolutely.  I've been meaning to do that for months (for a "shuffle"
  > program I want to add to coreutils), but hadn't gotten around to it.
  > It would have to be generalized a bit.  I'd like to have the file
  > descriptor cached, for example.

  I'll write a separate module for that part.

  I think we should even add a good PRNG that is re-seeded from
  /dev/?random frequently.  GnuTLS can need a lot of random data on a
  big server, more than /dev/random can supply.  And /dev/urandom might
  not be strong enough.  Further, the security of /dev/?random can also
  be questionable.

  >>   I'm also not sure about the names of those functions, they suggest
  >>   a more higher-level API than what is really offered (i.e., the
  >>   names "nonce" and "pseudo_random" and "random" imply certain
  >>   cryptographic properties).
  >
  > Could you expand a bit more on that?  What is the relationship between
  > nonce/pseudorandom/random and the /dev/ values you are using?

  There is none, that is the problem.

  Applications generally need different kind of "random" numbers.
  Sometimes they just need some random data and doesn't care whether it
  is possible for an attacker to compute the string (aka a "nonce").
  Sometimes they need data that is very difficult to compute (i.e.,
  computing it require inverting SHA1 or similar).  Sometimes they need
  data that is not possible to compute, i.e., it wants real entropy
  collected over time on the system.  Collecting the last kind of random
  data is very expensive, so it must not be used too often.  The second
  kind of random data ("pseudo random") is typically generated by
  seeding a good PRNG with a couple of hundred bytes of real entropy
  from the "real random" data pool.  The "nonce" is usually computed
  using the PRNG as well, because PRNGs are usually fast.

  Pseudo-random data is typically used for session keys.  Strong random
  data is often used to generate long-term keys (e.g., private RSA
  keys).

  Of course, there are many subtleties.  There are several different
  kind of nonce:s.  Sometimes a nonce is just an ever-increasing
  integer, starting from 0.  Sometimes it is assumed to be unlikely to
  be the same as previous nonces, but without a requirement that the
  nonce is possible to guess.  MD5(system clock) would thus suffice, if
  it isn't called too often.  You can guess what the next value will be,
  but it will always be different.

  The problem is that /dev/?random doesn't offer any kind of semantic
  guarantees.  But applications need an API that make that promise.

  I think we should do this in several steps:

  1) Write a module that can read from /dev/?random.

  2) Add a module for a known-good PRNG suitable for random number
  generation, that can be continuously re-seeded.

  3) Add a high-level module that provide various different randomness
  functions.  One for nonces, perhaps even different kind of nonces,
  one for pseudo random data, and one for strong random data.  It is
  not clear whether we can hope to achieve the last one in a portable
  way.

  Further, it would be useful to allow users to provide their own
  entropy source as a file, used to seed the PRNG or initialize the
  strong randomness pool.  This is used on embedded platforms that
  doesn't have enough interrupts to hope to generate good random data.

  > For example, why not use OpenBSD's /dev/arandom?

  I don't trust ARC4.  For example, recent cryptographic efforts
  indicate that you must throw away the first 512 bytes generated from
  the PRNG for it to be secure.  I don't know whether OpenBSD do this.
  Further, I recall some eprint paper on RC4 security that didn't
  inspire confidence.

  While I trust the random devices in OpenBSD more than
  Solaris/AIX/HPUX/etc, I think that since we need something better on
  Solaris/AIX/HPUX we'd might as well use it on OpenBSD or even Linux
  too.

  > Here is one thought.  The user could specify a desired quality level
  > range, and the implementation then would supply random data that is at
  > least as good as the lower bound of the range.  I.e., ihe
  > implementation refuses to produce any random data if it can't generate
  > data that is at least as good as the lower end of the range.  The
  > upper bound of the range is advice from the user not to be any more
  > expensive than that, but the implementation can ignore the advice if
  > it doesn't have anything cheaper.

  I'm not sure this is a good idea.  Users can't really be expected to
  understand this.  Further, applications need many different kind of
  random data.  Selecting the randomness level for each by the user will
  be too complicated.

  I think it is better if the application decide, from its cryptographic
  requirement, what entropy quality it require, and call the proper API.
  Meeting the implied semantic properties should be the job for gnulib.

  >> Perhaps gc_dev_random and gc_dev_urandom?
  >
  > To some extent.  I'd rather insulate the user from the details of
  > where the random numbers come from.  On the other hand we need to
  > provide a way for applications to specify a file that contains
  > random bits, so that people can override the defaults.

  Agreed.

  This may require some thinking before it is finalized.  Is it ok to
  install the GC module as-is meanwhile?  Then I can continue to add the
  stuff that GnuTLS need, and then come back to re-working the
  randomness module.  That way, we have two different projects that use
  the code.  GnuTLS includes the same randomness code that was in GNU
  SASL and that is in the current gc module.  I feel much more
  comfortable working in small steps at a time, rather then working on
  this for a long time in gnulib and only later integrate the stuff in
  GnuTLS.

  Thanks,
  Simon
 */

#endif /* GC_H */
