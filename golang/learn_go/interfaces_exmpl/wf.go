package main

import (
	"errors"
)

type WellsFargo struct {
	balance int
}

func NewWellsFargo() *WellsFargo {
	return &WellsFargo{
		balance: 0,
	}
}

func (w *WellsFargo) GetBalance() int {
	return w.balance
}

func (w *WellsFargo) Deposit(amount int) {
	w.balance += amount
}

func (w *WellsFargo) Withdraw(amount int) error {
	newBalance := w.balance - amount
	if newBalance < 0 {
		return errors.New("insufficient funds")
	}
	w.balance = newBalance
	return nil
}
