# The Interview Pitch (Simple English)

*Use this script when a professor, interviewer, or teammate asks you: "Can you explain your project?"*

---

## 1. The Short Pitch (30 Seconds)
"I built an Advanced ATM Simulator in C++. It’s a complete banking management system with a colorful command-line interface. Instead of just saving data to basic text files, I integrated a real **SQLite relational database** directly into the C++ code to store accounts and transaction histories. I also implemented industry-standard security by hashing user PINs using the **SHA-256 algorithm**. To demonstrate Object-Oriented Programming, I used Polymorphism to create different rules for Savings and Current accounts."

## 2. Explaining the Tech Stack (If they ask "How did you build it?")
"The project is entirely written in C++11. 
- For the database, I didn't want to rely on a heavy external MySQL server, so I used the **SQLite C-amalgamation**. This means the database engine compiles *with* my code, making it instantly portable.
- For security, I implemented my own lightweight **SHA-256 hashing class** to ensure PINs are never stored in plain text.
- For the UI, I used **ANSI escape codes** to draw beautiful, colorful borders and menus right inside the terminal."

## 3. Explaining OOP Concepts (Very Important)
*If they ask: "How did you use Object-Oriented Programming in this?"*

"I used all four pillars of OOP:
1. **Encapsulation**: Critical data like the account balance and PIN hash are kept private. They can only be modified through controlled functions like `deposit()` and `withdraw()`.
2. **Abstraction**: I abstracted all the complex SQL database queries into a `DatabaseManager` class, so the main system logic doesn't have to worry about SQL syntax.
3. **Inheritance**: I have a base `Person` class, and an abstract `Account` class inherits from it.
4. **Polymorphism**: This is the best part. A user can open a `SavingsAccount` or a `CurrentAccount`. Both inherit from `Account`, but they override the `withdraw()` function differently. Savings accounts cannot go below a zero balance, but Current accounts have an overdraft limit of $500. The system uses the exact same `withdraw()` command, but it behaves differently based on the account type."

## 4. Explaining Security (The "Wow" Factor)
*If they ask: "How is the data secured?"*

"First, I built a 3-strike locking system. If a user enters the wrong PIN three times, their account gets locked in the database and only the Admin can unlock it. Second, when a PIN is created, it is passed through a SHA-256 hashing algorithm. What gets saved in the database is a 64-character hash, not the real PIN. Even if a hacker stole the `bank.db` file, they could not read the users' PINs."

## 5. Teammate/Professor Walkthrough (Demo Steps)
When showing the project live to someone, do this:
1. Run the app. Show them the colorful UI.
2. Create a Savings Account. Show them the multi-currency balance.
3. Show them that typing letters instead of numbers doesn't crash the app (robust error handling).
4. Deliberately type the wrong PIN 3 times to show the Account Locking feature.
5. Log in as Admin (`admin` / `admin123`) and Unlock the account.
6. Open the `data/bank.db` file in a text editor to show them that it's a compiled binary database file and the PINs are completely scrambled (hashed), proving your security claims!
