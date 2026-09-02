# ATM Simulator (Bank Management System) — Simple Walkthrough

*A plain-language script for explaining your C++ project in an interview. It runs about 5–6 minutes at a normal speaking pace. This is your lead project for the C++ / LS-SWIFT roles, so know it well. Where I say "you likely," replace it with what you actually did in your code.*

---

## What it is

My project is an ATM Simulator, also called a Bank Management System. It's a program written in C++ that copies how a real ATM works. A person can create a bank account, log in with a PIN, deposit or withdraw money, check their balance, and transfer money to another account — all from the command line.

It has two kinds of users. A normal **User** can do their own banking. An **Admin** can look after the whole system and see all the accounts.

## Why I built it

I built it to practice object-oriented programming in C++ in a real, hands-on way. Banking is a good example, because it has clear objects — accounts, users, transactions — and it needs data to be stored safely and kept correct. It let me work with classes, file handling, and basic data security all in one project.

## The two modes

**User mode** is for the account holder. They log in and manage their own money.

**Admin mode** is for managing the system. The admin can view all accounts and export the account data to a CSV file, which is a simple spreadsheet format that can be opened in Excel. This is useful for keeping records.

## The main features

The program has eight core features:

1. **Create account** — make a new account with the user's details and a PIN.
2. **Login** — check the account number and PIN before letting anyone in.
3. **Deposit** — add money to the account.
4. **Withdraw** — take money out, after checking there's enough balance.
5. **Balance inquiry** — show the current balance.
6. **Money transfer** — move money from one account to another.
7. **PIN management** — let the user change their PIN.
8. **Logout** — safely end the session.

## How the data is stored

Since it's a simple project, I didn't use a big database. Instead, I used **text files as the database**. Each account's details are saved as records in a file, so the data stays even after the program closes. When the program starts, it reads the data back from the file. For the admin, I also added a CSV export so the account list can be opened as a spreadsheet.

## How I kept the data safe

Bank data is sensitive, especially PINs. So I added **basic encryption**. Encryption means changing the data into a scrambled form before saving it, so if someone opens the file directly, they can't read the real PIN or account details. When the program needs the data, it decrypts it back to normal. *(You likely used a simple method — describe the exact one you used. Be honest that it's basic, not bank-grade security.)*

## Object-oriented design

I built the program using object-oriented programming. I used **classes** to represent the main things in the system — for example, an account — and I used **inheritance** so related types could share common code. Keeping the account data private inside the class, and only letting it be changed through proper functions, is what kept the balance and PIN protected from accidental changes. *(Describe your actual classes and how they relate.)*

## Cross-platform

I made sure the program runs on both **Windows and Linux**. I handled the small differences between the two systems and added error handling, so if a user types something wrong — like letters instead of a number — the program gives a clear message instead of crashing.

## Challenges and what I'd improve

The trickiest part was making sure the data stayed correct — for example, during a money transfer, the amount has to leave one account and arrive in the other without any mistake. Getting the file reading and writing right also took care, so records didn't get corrupted.

If I took it further, I'd move from text files to a real database, add stronger and standard encryption, add automated tests, and maybe build a simple graphical interface instead of the command line.

Overall, this project gave me solid, practical experience with C++, object-oriented design, file handling, and thinking about data security.

---

## 30-second version (the quick pitch)

> "It's an ATM Simulator built in C++ using object-oriented programming. It has User and Admin modes and eight banking features like account creation, login, deposit, withdrawal, and money transfer. I stored the data in text files with CSV export for the admin, added basic encryption to protect PINs, and made it run on both Windows and Linux with proper error handling."
