-- Keep a log of any SQL queries you execute as you solve the mystery.
/* Commenting this in retrospect up here; I didn't really look at the style guide until I was done haha so it's really only
the final query that truly and properly follows all the style rules. I'll keep the style rules in mind going forward! */

/* Everything that we know off the bat is that the crime took place on *July 28 2023* at *Humphrey Street*. So we know we should
start our search somwhere here */

/* Let's start by checking the crime scene reports */
SELECT * FROM crime_scene_reports;

/* This gives A LOT of info. Let's first check if everything is from 2023 */
SELECT DISTINCT year FROM crime_scene_reports;
/* Everything is indeed from 2023. July is the 7th month. Let's see which months are in the reports */
SELECT DISTINCT month FROM crime_scene_reports;
/* The crime reports stretches as far back as the start of the year til the month after the crime. Interesting. Let's read
them one by one to get an understanding of everything that has happened and see what is relevant */
SELECT * FROM crime_scene_reports WHERE month=1;

/* I see, I see. This is a whole file of ALL the crimes in this city. That makes sense. Thus Everything before month 7 day 28
is effectively irrelevant. Let's check everything on this date and the days after */
SELECT * FROM crime_scene_reports WHERE month=7 AND DAY>=28;
/* Okay so it's clearly the crime with id 295 that is of most interest to us. Let's isolate the description */
SELECT description FROM crime_scene_reports WHERE id=295;
/* Theft of the CS50 duck took place at *10:15am* at Humphrey Street bakery. Interviews were conducted today with *three
witnesses* who were *present at the time* - each of their interview transcripts mentions the bakery. */

/* So new important info. We have a time of the crime and the number of witnesses who were also all present at the time
of the crime. Let's first look at the bakery security logs at the day of the crime to pick up more crucial information
and then compare it with the info of these three interviews to catch discrepancies! */
SELECT * FROM bakery_security_logs WHERE month=7 AND DAY=28 AND hour=10;
/* This reveals some VERY interesting information. TWO people entered... no, this is wrong thinking. I have to check the
activity of the bakery for the entire day it was open */
SELECT * FROM bakery_security_logs WHERE month=7 AND DAY=28;
/* My instinct was to say that it could only have been the person with license plate R3G7486 who entered at 10:08 or the
person with license plate 13FNH73 who entered at 10:14 but that's a lie. It could technically have been anyone that
entered on the 28th and *had not yet left the bakery by 10:15*. That's an important distinction. Let's try and see how
we can query all of these people */
SELECT hour, minute, activity, license_plate FROM bakery_security_logs WHERE month=7 AND DAY=28 AND hour<=10;
/* This gives all the entries where we are only interested in everything up until 10:15. We specifically want the entries
where the license plate only shows up an odd amount! Since even amount would mean they entered and left X amount of times */
SELECT hour, minute, activity, license_plate, COUNT(*) as count
FROM bakery_security_logs
WHERE month=7 AND DAY=28 AND hour<=10
GROUP BY license_plate
HAVING count % 2 = 1;
/* I thought this gave me the correct info until I manually checked the logs from the previous query which is what I will go
with
1. Person with license plate L93JTIZ; entered 8:18 and left 10:21. That checks out to be a prime suspect!!
2. Person with license plate 94KL13X; entered 8:23 and left 10:18. Another prime suspect!!
3. Person with license plate 1106N58; entered 8:34 and left 10:35. Yet another one, dang
4. Person with license plate 322W7JE; entered 8:36 and left 10:23. There are more than I expected
5. Person with license plate 0NTHK55; entered 8:42 and left 10:23. Same exit time, interesting
6. Person with license plate 4328GD8; entered 9:14 and left 10:19
7. Person with license plate 5P2BI95; entered 9:15 and left 10:16. Just one minute after the crime
8. Person with license plate 6P58WS2; entered 9:20 and left 10:18
9. Person with license plate G412CB7; entered 9:28 and left 10:20
10. Person with license plate R3G7486; entered 10:08 and left after 11, wow.
11. Person with license plate 13FNH73; entered 10:14 which is just one minute before the crime but still a suspect! It
could have been a really frickin quick fetch haha
We have a total of 11 unusual activities and suspects! But only 3 witnesses. Wait, that's not correct. That's just the
number of witnesses that were interviewed. Let's check these interviews! */

SELECT * FROM interviews WHERE month=7 AND day=28;
/* We do not have hour and minute for the interviews which is a shame. But let's write down what sticks out!
Ruth; "Sometime within ten minutes of the theft, I saw the thief get into a car in the bakery parking lot and drive away. If
you have security footage from the bakery parking lot, you might want to look for cars that left the parking lot in that time
frame". This checks out with... wait, I'm stupid. I've counted the suspects as the ones that entered the bakery but never left.
That means they never escaped. Let's re-check the security logs and re-write the suspects;
1.
Hmmmmm. There is not a single entry of someone who exited the bakery even near 10 minutes before the crime. But our suspects are:
1. Person with license plate O784M2U; entered 8:59 and left 8:59. This is the latest exit before the crime
2. Person with license plate 8LLB0B2; entered 8:57 and left 8:57
3. Person with license plate 4V16VO0; entered 8:50 and left 8:50
Before I write up every single suspect, let's just start with these and return to the interesting interviews */

/* Eugene; "I don't know the thief's name, but it was someone I recognized. Earlier this morning, before I arrived at Emma's bakery,
I was walking by the ATM on Leggett Street and saw the thief there withdrawing money
Before checking the rest of the interviews, let's follow this one through by first checking when Eugene arrived at Emma's bakery:
(I assume the bakery at Humphrey Street is Emma's Bakery) */

SELECT * FROM bakery_security_logs WHERE license_plate=
(SELECT license_plate FROM people WHERE name="Eugene");
/* This is interesting. If we are to assume that Emma's Bakery is the bakery for which we are viewing security logs from, Eugene
did not enter it on the day of the crime. The interesting entry here is two days before the crime during which she entered
Emma's Bakery at *13:22*. It's strange that the interview is from the 28th and she does say "This morning" when she didn't
enter the bakery that morning. But the only interview from the 26th is from a person named Steven, not Eugene. So I will
just go with this. Let's check ATM transactions on the 26th on Leggett Street before 13:22! */

SELECT * FROM atm_transactions WHERE month=7 AND day=26 AND atm_location="Leggett Street";
/* Once again, there is no hour or minute we can extract here. Let's get all distinct accounts: */
SELECT DISTINCT account_number FROM atm_transactions WHERE month=7 AND day=26 AND atm_location="Leggett Street";
SELECT COUNT(DISTINCT account_number) FROM atm_transactions WHERE month=7 AND day=26 AND atm_location="Leggett Street";
/* This gives us a list of 19 accounts! I think this is a dead end as of right now, let's return to the interviews */

SELECT * FROM interviews WHERE month=7 AND day=28;
/* Raymond; "As the thief was leaving the bakery, they called someone who talked to them for less than a minute. In the call,
I heard the thief say that they were planning to take the earliest flight out of Fiftyville tomorrow. The thief then asked
the person on the other end of the phone to purchase the flight ticket"
Where can we find information related to this? Let's check the phone_calls logs:
("as the thief was leaving the bakery" meaning the call was on the day of the crime) */
SELECT * FROM phone_calls WHERE month=7 AND day=28;
/* We have quite a few. But we know that the call only lasted for less than a minute! I assume duration here is in seconds: */
SELECT caller, receiver, duration FROM phone_calls WHERE month=7 AND day=28 AND duration <=60;
SELECT COUNT(*) FROM phone_calls WHERE month=7 AND day=28 AND duration <=60;
/* There is one call that is exactly one minute haha. I'll keep it for consideration for now. With that one included we have
10 phone calls. Hmmm..
AH! The caller has to match the person that withdrew money at Leggett Street, doesn't it? */
SELECT DISTINCT account_number FROM atm_transactions WHERE month=7 AND day=26 AND atm_location="Leggett Street";
SELECT caller, receiver, duration FROM phone_calls WHERE month=7 AND day=28 AND duration <=60;
/* Okay, we have to do a join the with bank_accounts table on account_number */
SELECT name, passport_number, license_plate
FROM people
JOIN bank_accounts ON people.id=bank_accounts.person_id
WHERE people.phone_number IN
(SELECT caller FROM phone_calls WHERE month=7 AND day=28 AND duration <=60);
/* Halfway there! Let's iterate to include the bank account match: */
SELECT name, passport_number, license_plate
FROM people
JOIN bank_accounts ON people.id=bank_accounts.person_id
WHERE people.phone_number IN
(SELECT caller FROM phone_calls WHERE month=7 AND day=28 AND duration <=60)
AND bank_accounts.account_number IN
(SELECT DISTINCT account_number FROM atm_transactions WHERE month=7 AND day=26 AND atm_location="Leggett Street");
/* LET'S GO!! We have 4 suspects;
1. Bruce with passport number 5773159633 and license_plate 94KL13X
2. Diana with passport number 3592750733 and license_plate 32W7JE
3. Kenny with passport number 9878712108 and license_plate 30G67EN
4. Benista with passport number 95866786673 and license_plate 8X428L0
That reminds that we can do a final match to see if they even entered Emma's Bakery on the day of the crime cuz I only
recognize some of these license plates: */
SELECT DISTINCT license_plate FROM bakery_security_logs WHERE month=7 AND DAY=28 AND hour<=10;
/* Final query: */
SELECT name, passport_number, license_plate
FROM people
JOIN bank_accounts ON people.id=bank_accounts.person_id
WHERE phone_number IN
(SELECT caller FROM phone_calls WHERE month=7 AND day=28 AND duration <=60)
AND bank_accounts.account_number IN
(SELECT DISTINCT account_number FROM atm_transactions WHERE month=7 AND day=26 AND atm_location="Leggett Street")
AND license_plate IN
(SELECT DISTINCT license_plate FROM bakery_security_logs WHERE month=7 AND DAY=28 AND hour<=10);
/* That leaves TWO suspects!!!!
1. Bruce with passport number 5773159633 and license_plate 94KL13X
2. Diana with passport number 3592750733 and license_plate 32W7JE
We're close! Where to now? We only have their passport to go by so let's go from there!
Let's first recall all the important details from the last interview:
In the call, I heard the thief say that they were planning to take the earliest flight out of Fiftyville tomorrow. The
thief then asked the person on the other end of the phone to purchase the flight ticket"
So the plan by the thief and his accomplice was to take the earliest flight the day after the crime. "The other person
on the phone" that bought the ticket is the accomplice so that's also an end to follow. But let's first check the final
interesting interview: */
SELECT * FROM interviews WHERE month=7 AND day=28;
/* Nevermind, I don't think the final interview is of interest. At least not now. Let's check the flights that left on the
29th of July: */

SELECT * FROM flights WHERE month=7 AND day=29 ORDER BY hour;
/* The earliest flight left at 8:20 and has id 36. Let's see where it flew */
SELECT * FROM airports WHERE id=4;
/* So this flight flew to LaGuardia Airport in New York City. Let's see if any of our two suspects where on this flight by
checking all the passports of the passengers against the two of the suspects'! */
SELECT * FROM passengers WHERE flight_id=36;
/* I see the row in question :)) But let's make a query out of it: */
SELECT name AS TheThief
FROM people
JOIN bank_accounts ON people.id=bank_accounts.person_id
JOIN passengers ON people.passport_number=passengers.passport_number
WHERE phone_number IN
(SELECT caller FROM phone_calls WHERE month=7 AND day=28 AND duration <=60)
AND bank_accounts.account_number IN
(SELECT DISTINCT account_number FROM atm_transactions WHERE month=7 AND day=26 AND atm_location="Leggett Street")
AND license_plate IN
(SELECT DISTINCT license_plate FROM bakery_security_logs WHERE month=7 AND DAY=28 AND hour<=10)
AND people.passport_number IN
(SELECT passengers.passport_number FROM passengers WHERE flight_id=36);
/* This is the first time I got the error "Parse error: ambiguous column name" (passport_number) but we got it! Singling
out the thief in a single query!! Bruce!!!
Let's get the rest of the info needed! */

/* What city the thief escaped to; since we identified that this is indeed the flight that the thief hopped on, we can
safely say that the thief escaped to *New York City* */

/* Who the thief’s accomplice is who helped them escape; This would be the other people in the line during the phone call!
Let's get the phone call query: */
SELECT caller, receiver, duration FROM phone_calls WHERE month=7 AND day=28 AND duration <=60;
/* Now we also know the number of the caller Bruce; */
SELECT phone_number AS PhoneNumberOfBruceTheThief
FROM people
JOIN bank_accounts ON people.id=bank_accounts.person_id
JOIN passengers ON people.passport_number=passengers.passport_number
WHERE phone_number IN
(SELECT caller FROM phone_calls WHERE month=7 AND day=28 AND duration <=60)
AND bank_accounts.account_number IN
(SELECT DISTINCT account_number FROM atm_transactions WHERE month=7 AND day=26 AND atm_location="Leggett Street")
AND license_plate IN
(SELECT DISTINCT license_plate FROM bakery_security_logs WHERE month=7 AND DAY=28 AND hour<=10)
AND people.passport_number IN
(SELECT passengers.passport_number FROM passengers WHERE flight_id=36);
/* It's (367) 555-5535. We can get the phone number of the accomplice with: */
SELECT receiver AS AccomplicePhoneNumber
FROM phone_calls
WHERE month=7 AND day=28 AND duration <=60 AND caller=
    (SELECT phone_number AS PhoneNumberOfBruceTheThief
    FROM people
    JOIN bank_accounts ON people.id=bank_accounts.person_id
    JOIN passengers ON people.passport_number=passengers.passport_number
    WHERE phone_number IN
    (SELECT caller FROM phone_calls WHERE month=7 AND day=28 AND duration <=60)
    AND bank_accounts.account_number IN
    (SELECT DISTINCT account_number FROM atm_transactions WHERE month=7 AND day=26 AND atm_location="Leggett Street")
    AND license_plate IN
    (SELECT DISTINCT license_plate FROM bakery_security_logs WHERE month=7 AND DAY=28 AND hour<=10)
    AND people.passport_number IN
    (SELECT passengers.passport_number FROM passengers WHERE flight_id=36));

/* And with a final single query we can get the name of the accomplice with: */
SELECT name AS accomplice_name
  FROM people
 WHERE phone_number =
       (SELECT receiver --AS accomplice_phone_number
          FROM phone_calls
         WHERE month = 7 AND day = 28 AND duration <= 60 AND caller =
               (SELECT phone_number --AS thief_phone_number
                  FROM people AS p1
                  JOIN bank_accounts AS b
                    ON p1.id = b.person_id
                  JOIN passengers AS p2
                    ON p1.passport_number = p2.passport_number
                 WHERE phone_number IN
                       (SELECT caller
                          FROM phone_calls
                         WHERE month = 7 AND day = 28 AND duration <= 60)
                   AND b.account_number IN
                       (SELECT DISTINCT account_number
                          FROM atm_transactions
                         WHERE month = 7 AND day = 26 AND atm_location = "Leggett Street")
                   AND license_plate IN
                       (SELECT DISTINCT license_plate
                          FROM bakery_security_logs
                         WHERE month = 7 AND DAY = 28 AND hour <= 10)
                   AND p1.passport_number IN
                       (SELECT passengers.passport_number
                          FROM passengers
                         WHERE flight_id = 36)));
