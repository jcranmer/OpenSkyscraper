OpenSkyscraper TODO
===================


Resources
---------

Implement the [KWAJ compression](http://www.cabextract.org.uk/libmspack/doc/szdd_kwaj_format.html) scheme so we're able to extract the resources even from a compressed variant of SimTower, i.e. before running the `SETUP.EXE` which would greatly benefit Linux and Mac users.

As an addition, we could also implement reading the `SIMTOWER.EX_` directly from some form of .zip archive, then decompressing the `*.EX_` in memory and reading the resources. Just to proof that open source stuff really rocks :).


Tower
-----

- Add some methods to find the closest transport item.


### Decoration

- Move the drawing of the entrances of the tower from the lobby class to the tower decoration.


### Structure

- At the moment it is possible to build two unconnected items on a floor. This was not the case in SimTower, so we should modify the `constructItem()` function to check whether there already is a cell on that floor. If there is, the the distance between it and the construction rect needs to be filled with floor items. Note that `getCell(int2)` will return
a cell if there is at least a floor cell at this location. So DON'T use `getItems(int)` to find any pre-existing stuff on that floor; use `getCells(int)` instead.


Facilities
----------

- When two adjacent items are collapsed, they both show the construction animation. It should be possible to only show the animation for the part that was actually added to cause the collapse.
- Create classes for all items of SimTower.
- Add people to the office item and animate them appropriately.
- Add people to all other items.
- Add a mechanism that checks whether a facility is accessible from the lobby or not.
- Develop an algorithm to find the facilities that people from other facilities visit during the day. This includes Fast Foods for Office workers, or Restaurants for Hotel guests or Condo inhabitants.


### Items to be created

- Cinema, `CinemaItem`, `facilities/entertainment/cinema.h|.cpp`
- Parking Ramp, `ParkingRampItem`, `facilities/infrastructure/parkingramp.h|.cpp`
- Parking Space, `ParkingSpaceItem`, `facilities/infrastructure/parkingspace.h|.cpp`
- Recycling Center, `RecyclingCenterItem`, `facilities/infrastructure/recyclingcenter.h|.cpp`
- Metro Station, `MetroStationItem`, `facilities/infrastructure/metrostation.h|.cpp`
- Cathedral, `CathedralItem`, `facilities/services/cathedral.h|.cpp`
- Security, `SecurityItem`, `facilities/services/security.h|.cpp`
- Medical Center, `MedicalCenterItem`, `facilities/services/medicalcenter.h|.cpp`
- Condo, `CondoItem`, `facilities/condo/condo.h|.cpp`


### Items requiring simulation and mechanics

- Fast Food
- Party Hall
- Restaurant
- Shop
- Office


Transports
----------

### Elevators

- Fix the size of the queue that is being drawn. It should be limited by elevators nearby as well as the floor bounds. The best thing would be to calculate the rect depending on the surrounding facilities, and then doing an intersection with the floor rect.

- Adjust the elevator speeds since we now have the new time measurements which jcranmer provided. There's now also a possibility to actually calculate the elevator waiting time in seconds as it was in the original game.


People
------

### Hotel Guest

- **ASAP**: The hotel guest uses the `setPauseEndTimeToday()` and similar functions to do the timing. This has some annoying implications. For example will a guest sleep for 30 hours if he/she fails to set the wake up time before 1:30 in the morning. After 1:30 the next day will be selected which leads to loooooong dreams. One way to counteract this would be to create a `setPauseEndTimeRelative()` function in conjunction with `setTimeAnchor()`. As soon as the person for moving into the hotel is created, it sets its time anchor to the current day's 0:00. All later timing calculations are done relative to that. This will lead to sane wake up times even if the guest fails to go to sleep before 1:30, since the time anchor is still set to the previous day.


User Interface
--------------

### Tools Window

- Create the bulldozer, hand and inspection tools.


### Control Window

- Add funds and population values
- Add weekday, date, quarter and year information


### Map Window

The map window should show a thumbnail of the tower and some buttons to activate overlay. This has **low priority** until the inner workings of the simulation are done.


GUI
---

Maybe we should use CEGUI?


Audio
-----

At the moment, the entire game is suffering somewhat from audio overkill. Every single elevator emits a sound when departing and arriving. This leads to incredible noise with as few as 3 elevators on a busy hotel night. We need some way of filtering these things. Maybe we should keep a list of similar sounds and fade older ones if there are too many new. This would reduce the overall noise level.
