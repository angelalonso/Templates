package main

import (
	"fmt"
	"io/ioutil"
	"net/http"
	"context"
	"time"
)

func main() {
	// Add a timeout to the request
	timeoutContext, cancel := context.WithTimeout(context.Background(), time.Millisecond * 100)
	defer cancel()

	// create HTTP request
	//req, err := http.NewRequest(http.MethodGet, "http://placehold.it/2000x2000", nil)
	req, err := http.NewRequestWithContext(timeoutContext, http.MethodGet, "http://placehold.it/2000x2000", nil)
	if err != nil { 
		panic(err) 
	}

	// Perform HTTP request
	res, err := http.DefaultClient.Do(req)
	if err != nil { 
		panic(err) 
	}
	defer res.Body.Close()

	// Get data from HTTP response
	imageData, err := ioutil.ReadAll(res.Body)

	fmt.Printf("downloaded image of size %d\n", len(imageData))

}
