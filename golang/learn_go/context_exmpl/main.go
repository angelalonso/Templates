package main

import (
	"context"
	"fmt"
	"log"
	"time"
)
func main() {
	start := time.Now()
	//ctx := context.Background()
	ctx := context.WithValue(context.Background(), "foo", "bar") // version with variables
	userID := 10
	val, err := fetchUserData(ctx, userID)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("result: ", val)
	fmt.Println("took: ", time.Since(start))

}

type Response struct {
	value int
	err error
}

func fetchUserData(ctx context.Context, userID int) (int, error) {
	val := ctx.Value("foo") // version with variables
	fmt.Println(val) // version with variables
	ctx, cancel := context.WithTimeout(ctx, time.Millisecond * 200)
	defer cancel() // Otherwise we'd have leaks

	respch := make(chan Response)

	go func() {
		val, err := fetchThirdPartyStuffWhichCanBeSlow()
		respch <- Response {
			value: val,
			err: err,
		}
	}()

	for {
		select {
		case <- ctx.Done():
			return 0, fmt.Errorf("third party response took too long")
		case response := <- respch:
			return response.value, response.err
		}
	}
}

func fetchThirdPartyStuffWhichCanBeSlow() (int, error) {
	time.Sleep(time.Millisecond * 199)
	return 666, nil
}
