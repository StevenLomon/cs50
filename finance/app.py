import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from sqlalchemy.exc import DatabaseError, IntegrityError, OperationalError, DataError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""
    user = db.execute("SELECT username FROM users WHERE id = :user_id", user_id=session["user_id"])
    if user:
        user = user[0]['username']
    else:
        return apology("could not retrieve username from logged in user", 400)

    # Fetch the latest cash amount for the logged in user from the database
    cash = db.execute("SELECT cash FROM users WHERE username = ?", user)
    if cash:
        cash = cash[0].get('cash')
    else:
        return apology("could not retrieve cash amount from logged in user", 400)

    # transactions = db.execute("SELECT * FROM transactions WHERE username = ?", user)
    # agg_buy = db.execute(
    #     "SELECT symbol, SUM(shares) AS total_shares FROM transactions WHERE username= ? AND type='buy' GROUP BY symbol", user)
    # agg_sell = db.execute(
    #     "SELECT symbol, SUM(shares) AS total_shares FROM transactions WHERE username= ? AND type='sell' GROUP BY symbol", user)
    # aggregates_total = db.execute(
    #     "SELECT symbol, agg_buy.total_shares - agg.sell_total_shares AS total_shares FROM agg_buy JOIN agg_sell ON agg_buy.symbol = agg_sell.symbol WHERE username= ? GROUP BY symbol", user)
    # print(aggregates_total)

    rendered_transactions = []
    aggregates_total = db.execute(
        """
        SELECT buy.symbol, (buy.total_shares - IFNULL(sell.total_shares, 0)) AS total_shares
        FROM
            (SELECT symbol, SUM(shares) AS total_shares FROM transactions WHERE type='buy' AND username = ? GROUP BY symbol) AS buy
        LEFT JOIN
            (SELECT symbol, SUM(shares) AS total_shares FROM transactions WHERE type='sell' AND username = ? GROUP BY symbol) AS sell
        ON buy.symbol = sell.symbol
        """, user, user
    )

    for aggregate in aggregates_total:
        symbol = aggregate.get("symbol")
        shares = aggregate.get("total_shares")
        current_price = lookup(symbol).get("price")
        total_value = shares * current_price
        rendered_transactions.append({'symbol': symbol, 'shares': shares,
                                      'current_price': current_price, 'total_value': total_value})

    grand_total = sum([rendered_transaction.get('total_value')
                      for rendered_transaction in rendered_transactions]) + cash
    return render_template("index.html", cash=cash, transactions=rendered_transactions, grand_total=grand_total)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    # User reached route via POST (asking for stock info via POST)
    if request.method == "POST":
        # Ensure symbol was submitted
        symbol = request.form.get("symbol")
        if not symbol:
            return apology("must provide symbol", 400)

        # Ensure valid symbol and that price is available for the valid symbol
        try:
            price = lookup(symbol)["price"]
        except TypeError:
            return apology("must provide a valid symbol", 400)
        if not price:
            return apology(f"price not available for {symbol}", 400)

        # Ensure shares were submitted and valid
        shares = request.form.get("shares")
        if not shares or not shares.isdigit() or int(shares) < 1:
            return apology("must provide a valid amount of shares as integer", 400)
        shares = int(shares)

        total_cost = price * shares

        # Get cash amount from the current logged in user and current price of the desired stock
        cash = db.execute("SELECT cash FROM users WHERE id = :user_id", user_id=session["user_id"])
        if cash:
            cash = cash[0]['cash']
        else:
            return apology("could not retrieve cash from logged in user", 400)

        # See if user has enough cash
        if total_cost > cash:
            return apology("insufficient funds to complete purchase", 400)

        # Try to insert the transaction into the database
        user = db.execute("SELECT username FROM users WHERE id = :user_id",
                          user_id=session["user_id"])
        if user:
            user = user[0]['username']
        else:
            return apology("could not retrieve username from logged in user", 400)
        try:
            db.execute(
                "INSERT INTO transactions (username, type, symbol, price, shares) VALUES (?, ?, ?, ?, ?)",
                user, "buy", symbol, price, shares)
        except IntegrityError:
            return apology("integrity error. try again later", 400)
        except DataError:
            return apology("data error. try again later", 400)
        except OperationalError:
            return apology("operational error. try again later", 400)
        except DatabaseError:
            return apology("database error. try again later", 400)
        except:
            return apology("unexpected exception. try again later or contact us", 400)

        # Try to update cash amount for current user
        cash_new = cash - total_cost
        if not cash_new:
            return apology("could not calculate new cash amount for user, try again later", 400)

        user_id = db.execute("SELECT id FROM users WHERE id = :user_id", user_id=session["user_id"])
        if user_id:
            user_id = user_id[0]['id']
        else:
            return apology("could not retrieve id from logged in user", 400)

        try:
            db.execute("UPDATE users SET cash = ? WHERE id = ?", cash_new, user_id)
        except IntegrityError:
            return apology("integrity error. try again later", 400)
        except DataError:
            return apology("data error. try again later", 400)
        except OperationalError:
            return apology("operational error. try again later", 400)
        except DatabaseError:
            return apology("database error. try again later", 400)
        except:
            return apology("unexpected exception. try again later or contact us", 400)

        # print(f"Success! Bought {shares} shares from {symbol} for a total cost of {total_cost}")
        # print(f"Cash before: {cash}")
        # print(f"Cash after: {cash_new}")

        # Flash a success message to the user
        flash(f"Succesfully bought {shares} shares from {symbol} for a total cost of {total_cost}!")

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking the link in the navbar or via redirect)
    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    user = db.execute("SELECT username FROM users WHERE id = :user_id", user_id=session["user_id"])
    if user:
        user = user[0]['username']
    else:
        return apology("could not retrieve username from logged in user", 400)

    transactions = db.execute("SELECT * FROM transactions WHERE username = ?", user)
    rendered_transactions = []

    for transaction in transactions:
        symbol = transaction.get("symbol")
        type = transaction.get("type")
        shares = transaction.get("shares")
        price = transaction.get("price")
        date = transaction.get("timestamp")
        rendered_transactions.append({'symbol': symbol, 'type': type, 'shares': shares,
                                      'price': price, 'date': date})
    return render_template("history.html", transactions=rendered_transactions)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 400)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 400)

        # Query database for username
        rows = db.execute(
            "SELECT * FROM users WHERE username = ?", request.form.get("username")
        )

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(
            rows[0]["hash"], request.form.get("password")
        ):
            return apology("invalid username and/or password", 400)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    # User reached route via POST (asking for stock info via POST)
    if request.method == "POST":
        # Ensure symbol was submitted
        symbol = request.form.get("symbol")
        if not symbol:
            return apology("must provide symbol", 400)

        # Ensure symbol is valid (if we get a KeyError meaning the submitted symbol is not in the
        # Yahoo Finance API, the lookup function returns None)
        try:
            price = lookup(symbol)["price"]
        except TypeError:
            return apology("must provide a valid symbol", 400)

        return render_template("quoted.html", symbol=symbol, price=price)

    # User reached route via GET (as by clicking the link in the navbar or via redirect)
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    # User reached route via POST (submitting the registration form via POST)
    if request.method == "POST":
        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 400)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 400)

        # Ensure that passwords match
        elif not request.form.get("password") == request.form.get("confirmation"):
            return apology("the provided passwords do not match", 400)

        # Try to insert the user into the database. A ValueError exception will be raised if we try to INSERT a duplicate username
        try:
            db.execute(
                "INSERT INTO users (username, hash) VALUES (?, ?)",
                request.form.get("username"), generate_password_hash(request.form.get("password"))
            )
        except ValueError:
            return apology("username already taken :(", 400)

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking the Register link in the navbar or via redirect)
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    # Fetch username of currently logged in user regardless if the method is POST or GET
    user = db.execute("SELECT username FROM users WHERE id = :user_id", user_id=session["user_id"])
    if user:
        user = user[0]['username']
    else:
        return apology("could not retrieve username from logged in user", 400)

    if request.method == "POST":
        # Ensure symbol was submitted
        symbol = request.form.get("symbol")
        if not symbol:
            return apology("must provide symbol", 400)

        # Ensure valid symbol and that price is available for the valid symbol
        try:
            price = lookup(symbol)["price"]
        except TypeError:
            return apology("must provide a valid symbol", 400)
        if not price:
            return apology(f"price not available for {symbol}", 400)

        # Ensure shares were submitted and valid
        shares = request.form.get("shares")
        if not shares or not shares.isdigit() or int(shares) < 1:
            return apology("must provide a valid amount of shares as integer", 400)
        shares = int(shares)

        # Get total shares amount for the selected symbol from the current logged in user
        user_shares = db.execute(
            "SELECT SUM(shares) AS total_shares FROM transactions WHERE username = ? AND symbol = ?", user, symbol)
        if user_shares:
            user_shares = user_shares[0]['total_shares']
        else:
            return apology("could not fetch total shares from logged in user", 400)
        user_shares = int(user_shares)

        # See if user has enough shares
        if shares > user_shares:
            return apology("insufficient shares to complete sale", 400)

        # Try to insert the transaction into the database
        user = db.execute("SELECT username FROM users WHERE id = :user_id",
                          user_id=session["user_id"])
        if user:
            user = user[0]['username']
        else:
            return apology("could not retrieve username from logged in user", 400)
        try:
            db.execute(
                "INSERT INTO transactions (username, type, symbol, price, shares) VALUES (?, ?, ?, ?, ?)",
                user, "sell", symbol, price, shares)
        except:
            return apology("unexpected exception. try again later or contact us", 400)

        # Try to update cash amount for current user
        # Get cash amount from the current logged in user and current price of the desired stock
        cash = db.execute("SELECT cash FROM users WHERE id = :user_id", user_id=session["user_id"])
        if cash:
            cash = cash[0]['cash']
        else:
            return apology("could not retrieve cash from logged in user", 400)

        total_profit = price * shares
        cash_new = cash + total_profit
        if not cash_new:
            return apology("could not calculate new cash amount for user, try again later", 400)

        user_id = db.execute("SELECT id FROM users WHERE id = :user_id", user_id=session["user_id"])
        if user_id:
            user_id = user_id[0]['id']
        else:
            return apology("could not retrieve id from logged in user", 400)

        try:
            db.execute("UPDATE users SET cash = ? WHERE id = ?", cash_new, user_id)
        except:
            return apology("unexpected exception. try again later or contact us", 400)

        # Flash a success message to the user
        flash(
            f"Succesfully sold {shares} shares from {symbol} for a total profit of {total_profit}!")

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking the link in the navbar or via redirect)
    else:
        symbols = db.execute("SELECT DISTINCT symbol FROM transactions WHERE username = ?", user)
        return render_template("sell.html", symbols=symbols)


@app.route("/add_cash", methods=["GET", "POST"])
@login_required
def add_cash():
    """Allow users to add additional cash to their account"""
    # User reached route via POST (user used the form and submit)
    if request.method == "POST":
        # Ensure cash amount was submitted
        cash = request.form.get("cash")

        def valid_cash(cash):
            valid = True
            for char in cash:
                if not char.isdigit() and char not in [",", "."]:
                    return False
            return valid

        if not cash or not valid_cash(cash) or float(cash) <= 0:
            return apology("must provide a valid amount of cash", 400)
        cash = float(cash)

        # Get old cash amount from the current logged in user
        cash_old = db.execute("SELECT cash FROM users WHERE id = :user_id",
                              user_id=session["user_id"])
        if cash_old:
            cash_old = cash_old[0]['cash']
        else:
            return apology("could not retrieve cash from logged in user", 400)

        # Try to update cash amount for current user
        cash_new = cash_old + cash
        if not cash_new:
            return apology("could not calculate new cash amount for user, try again later", 400)

        user_id = db.execute("SELECT id FROM users WHERE id = :user_id", user_id=session["user_id"])
        if user_id:
            user_id = user_id[0]['id']
        else:
            return apology("could not retrieve id from logged in user", 400)

        try:
            db.execute("UPDATE users SET cash = ? WHERE id = ?", cash_new, user_id)
        except:
            return apology("unexpected exception. try again later or contact us", 400)

        # Flash a success message to the user
        flash(f"Succesfully added ${cash} to wallet!")

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking the link in the navbar or via redirect)
    else:
        return render_template("add_cash.html")


@app.route("/change_pw", methods=["GET", "POST"])
@login_required
def change_pw():
    """Allow the user to change password"""
    # User reached route via POST (entered old and new password via form submission)
    if request.method == "POST":
        # Ensure passwords were submitted
        pw_old = request.form.get("pw_old")
        pw_new = request.form.get("pw_new")
        pw_match = request.form.get("pw_match")

        if not pw_old or not pw_match or not pw_new:
            return apology("one or more passwords were not entered", 400)

        if not pw_new == pw_match:
            return apology("passwords does not match, try again", 400)

        # Try to update password for current user
        user_id = db.execute("SELECT id FROM users WHERE id = :user_id", user_id=session["user_id"])
        if user_id:
            user_id = user_id[0]['id']
        else:
            return apology("could not retrieve id from logged in user", 400)

        try:
            db.execute("UPDATE users SET hash = ? WHERE id = ?",
                       generate_password_hash(pw_new), user_id)
        except:
            return apology("unexpected exception. try again later or contact us", 400)

        # Flash a success message to the user
        flash(f"Succesfully changed password!")

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking the link in the navbar or via redirect)
    else:
        return render_template("change_pw.html")
