package main

import (
	"fmt"
)

func foo() {
//	defer fmt.Println("Done!") // evaluated immediately, but only run when the rest of func foo is done
//	defer fmt.Println("Are we done?") // this one runs before the one above
//	fmt.Println("Stuff being done here")
	for i := 0; i < 5; i++ {
		defer fmt.Println(i)
	}	
}

func main() {
	foo()
}
