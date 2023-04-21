package main

import (
	"fmt"
)

type IBankAccount interface {
	GetBalance() int // 100 = 1 Dollar
	Deposit(amount int)
	Withdraw(amoun int) error
}

func main() {
	myAccounts := []IBankAccount{
		NewWellsFargo(),
		NewBitcoinAccount(),
	}

	for _, account := range myAccounts {
		account.Deposit(500)
		if err := account.Withdraw(70); err != nil {
			fmt.Printf("ERR: %d\n", err)
		}
		balance := account.GetBalance()
		fmt.Printf("balance: %d\n", balance)
	}
}
