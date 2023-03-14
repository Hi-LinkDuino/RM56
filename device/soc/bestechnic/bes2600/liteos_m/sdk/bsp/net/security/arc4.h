#ifndef __ARC4_H__
#define __ARC4_H__

struct arc4_ctx {
	u32 S[256];
	u32 x, y;
};

extern int arc4_set_key(struct crypto_tfm *tfm, const u8 *in_key,
			unsigned int key_len);
extern void arc4_crypt_one(struct crypto_tfm *tfm, u8 *out, const u8 *in);
#endif

