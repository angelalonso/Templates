package main

import (
	"fmt"
	"time"
)

func main() {
	one := make(chan string)
	two := make(chan string)

	go func() {
		time.Sleep(time.Second * 2)
		one <- "Hey"
	}()

	go func() {
		time.Sleep(time.Second * 1)
		two <- "Hello"
	}()

	//// Wait on the receiving end and answer with the first one to get data
	//select {
	//case rec1 := <-one:
	//	fmt.Println("I received from Channel one", rec1)
	//case rec2 := <-two:
	//	fmt.Println("I received from Channel two", rec2)
	//}

	// Wait on all
	//   This could also be a simple for, similar to a while true, but then we need another channel to be able to quit/cancel (case <-quit, return)
	for x := 0; x < 2; x++ {
		select {
		case rec1 := <-one:
			fmt.Println("I received from Channel one", rec1)
		case rec2 := <-two:
			fmt.Println("I received from Channel two", rec2)
		}
	}
}
