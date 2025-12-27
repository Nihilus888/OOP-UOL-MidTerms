3-Minute System Demonstration Script
[0:00 – 0:25] Introduction & Code Overview

“Hi, my name is Elliott, and this is my CM2005 Object Oriented Programming coursework.
The goal of this project is to build a technical analysis toolkit for a currency exchange platform, using object-oriented design and CSV-based persistence.

I’ll first briefly walk through the code structure, then demonstrate the key features corresponding to Tasks 1 through 5.”

(Show IDE: MerkelMain, OrderBook, UserService, Wallet, CSVReader)

“The system is modular:
OrderBook handles market data and candlestick aggregation,
UserService manages registration and login,
Wallet tracks balances,
and MerkelMain coordinates user interaction through a menu-driven interface.”

[0:25 – 1:00] Task 2 – User Login & Registration

(Run program)

“When the program starts, the user must either register or log in.

During registration, the system requests a full name, email, and password. A unique 10-digit username is generated automatically, and the password is stored as a hashed value using std::hash before being written to a temporary CSV file.”

(Show login success)

“On login, the entered password is hashed and compared against the stored value.
This ensures credentials are never stored in plain text.”

[1:00 – 1:30] Task 5 – User Menu & Input Validation

(Show menu options)

“After logging in, the user is presented with a menu supporting all trading operations.

All user inputs are validated — for example, invalid menu options, malformed trade inputs, or insufficient wallet balances are handled gracefully without crashing the program.”

(Briefly type an invalid option)

“Incorrect inputs simply return an error message and allow the user to continue.”

[1:30 – 2:00] Task 1 – Candlestick Data & Market Analysis

(Select candlestick option)

“For Task 1, the system allows the user to view candlestick summaries for any product, such as ETH/USDT.

By default, yearly data is displayed, but the user can also choose daily or monthly timeframes.”

(Show output)

“Each candlestick contains open, high, low, and close values, computed by aggregating order book entries within the selected timeframe.
Asks and bids are handled separately using a dedicated candlestick class.”

[2:00 – 2:25] Task 3 – Wallet & Transaction History

(Show wallet, deposit, withdraw)

“The wallet supports deposits and withdrawals, which are logged to a transaction CSV file along with a timestamp.

The user can also view recent transactions and a summary of their activity, including the number of bids and asks placed and total spending.”

[2:25 – 2:50] Task 4 – Simulated Trading Activity

(Select simulated trading)

“For Task 4, the system simulates trading activity by generating five bid and five ask orders for every available product.

Prices are calculated relative to the most recent known market prices, with small percentage offsets applied to avoid unrealistic values.”

(Show transactions.csv briefly)

“All simulated trades use the current system timestamp and are appended persistently to the transaction log.”

[2:50 – 3:00] Closing

“Overall, this system demonstrates object-oriented design, robust input handling, CSV-based persistence, and technical market analysis features.

Thank you.”