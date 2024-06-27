import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session

# We won't care about leap year for validation
MONTHS_FROM_TEXT = {"january": 1,
                    "february": 2,
                    "march": 3,
                    "april": 4,
                    "may": 5,
                    "june": 6,
                    "july": 7,
                    "august": 8,
                    "september": 9,
                    "october": 10,
                    "november": 11,
                    "december": 12,}

MONTHS_TO_DAYS = {1: 31,
                  2: 28,
                  3: 31,
                  4: 30,
                  5: 31,
                  6: 30,
                  7: 31,
                  8: 31,
                  9: 30,
                  10: 31,
                  11: 30,
                  12: 31}

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///birthdays.db")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/", methods=["GET", "POST"])
def index():
    if request.method == "POST":

        # TODO: Add the user's entry into the database

        # Validate data
        name = request.form.get("name")
        month = request.form.get("month")
        day = int(request.form.get("day"))

        if not name:
            return render_template("error.html", message="Missing name!")

        if not month:
            return render_template("error.html", message="Missing month!")
        # Once we've made sure we have a month, we check if the user has entered the month as number or text
        if len(request.form.get("month")) < 3: # The month is entered as a number
            month = int(month) # This is what is used to validate the day input
            if month < 1 or month > 12:
                return render_template("error.html", message="Invalid month!")
        else: # The month is entered as a text
            if not month.lower() in MONTHS_FROM_TEXT: # The user did not enter a valid month
                return render_template("error.html", message="Invalid month! Type the full month")
            else: # Now we have the month as a valid text, and we need to convert to int
                month = MONTHS_FROM_TEXT[month.lower()] # Once again, this is what is used to validate the day input

        if not day:
            return render_template("error.html", message="Missing day!")
        if day < 1 or day > MONTHS_TO_DAYS[month]: # Once we reach this line of code, the month is validated
            return render_template("error.html", message="Invalid day!")

        # Insert into database
        db.execute("INSERT INTO birthdays (name, month, day) VALUES (?, ?, ?)", name, month, day)

        return redirect("/")

    else:

        # TODO: Display the entries in the database on index.html
        birthdays = db.execute("SELECT * FROM birthdays")
        return render_template("index.html", birthdays=birthdays)

@app.route("/remove", methods=["POST"])
def remove():
    # Remove birthday by first fetching the relevant id of the row to be deleted
    id = request.form.get("id")
    if id:
        db.execute("DELETE FROM birthdays WHERE id = ?", id)
    return redirect("/")
