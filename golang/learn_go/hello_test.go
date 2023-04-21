package main

import "testing"
import "fmt"

func TestHello(t *testing.T) {
	t.Run("saying hello to people", func(t *testing.T) {
		got := Hello("Dude", "")
		want := "Hello, Dude"
		assertCorrectMessage(t, got, want)

	})
	t.Run("saying hello world if no name is provided", func(t *testing.T) {
		got := Hello("", "")
		want := "Hello, World"
		assertCorrectMessage(t, got, want)
	})
	t.Run("in Spanish", func(t *testing.T) {
		got := Hello("Elodie", "Spanish")
		want := "Hola, Elodie"
		assertCorrectMessage(t, got, want)
	})
}

func assertCorrectMessage(t testing.TB, got, want string) {
	t.Helper()
	if got != want {
		t.Error(fmt.Sprintf("got %q want %q", got, want))
	}
}
