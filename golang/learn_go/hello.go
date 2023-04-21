package main

import "fmt"

const spanish = "Spanish"
const french = "French"
const englishHello = "Hello, "
const spanishHello = "Hola, "
const frenchHello = "Bonjour, "

func Hello(name string, language string) string {
	if name == "" {
		name = "World"
	}

	prefix := englishHello
	switch language {
	case french:
		prefix := frenchHello
	case spanish:
		prefix := spanishHello
	}
	return prefix + name
}

func main() {
	fmt.Println(Hello("", ""))
}
