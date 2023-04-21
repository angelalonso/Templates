package crypto

import (
	"io"
	"crypto/rand"
	"testing"
	"github.com/stretchr/testify/assert"
	"encoding/hex"
)

func TestGeneratePrivateKey(t *testing.T) {
  privKey := GeneratePrivateKey()
  assert.Equal(t, len(privKey.Bytes()), privKeyLen)
  pubKey := privKey.Public()
  assert.Equal(t, len(pubKey.Bytes()), pubKeyLen)
}

func TestPrivateKeySign(t *testing.T) {
  privKey := GeneratePrivateKey()
  pubKey := privKey.Public()
  msg := []byte("foo bar baz")

  sig := privKey.Sign(msg)

  assert.True(t, sig.Verify(pubKey, msg))
  assert.False(t, sig.Verify(pubKey, []byte("foo")))

  invalid_pk := GeneratePrivateKey()
  assert.False(t, sig.Verify(invalid_pk.Public(), msg))
}

func TestNewPrivateKeyFromString(t *testing.T) {
  seed := make([]byte, 32)
  io.ReadFull(rand.Reader, seed)
  privKey := NewPrivateKeyFromString(hex.EncodeToString(seed))
  assert.Equal(t, privKeyLen, len(privKey.Bytes()))
  address := privKey.Public().Address()
  assert.Equal(t, hex.EncodeToString(address.value), address.String())

}

func TestPublicKeyToAddress(t *testing.T) {
  privKey := GeneratePrivateKey()
  pubKey := privKey.Public()
  address := pubKey.Address()

  assert.Equal(t, addressLen, len(address.Bytes()))
}
