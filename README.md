# MissileTracker

## Here is a list of everything that this program does, in order

1. Sets up the "temp" folder. If the "temp" folder:\
       • Exists, then delete everything inside of it\
       • DOES NOT exist, then create an empty "temp" folder

2. Downloads this excel file into the "temp" folder\
       • https://www.nti.org/wp-content/uploads/2021/10/north_korea_missile_test_database.xlsx

3. Stores all rows into a vector<Row> called "entireTable", that meet these conditions\
       • Cell value != "Unknown"\
       • Missile launch date happens in 2020 or later

4. Creates a SQLite database file called "output.db", and stores it here\
       • Inside of the "temp" folder

5. Runs these SQLite queries\
       • CREATE TABLE, to create the "MissileLaunches" table\
       • INSERT INTO, to copy the "entireTable" vector into the newly-created "MissileLaunches" table\
       • "SELECT * FROM MissileLaunches ORDER BY distanceTraveled_km DESC", to make sure that the longest distance traveled is on top

6. Displays the entire table\
       • Below is what is looks like
