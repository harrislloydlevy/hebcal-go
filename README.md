# hebcal-go

This is a fork of the hebcal-go library to port to C and make useful as a component in arduino/esp32 and esphome builds.

The real action is going on in a branch: https://github.com/harrislloydlevy/hebcal-go/tree/hebcal-c


[![GoDoc](https://pkg.go.dev/badge/github.com/hebcal/hebcal-go?status.svg)](https://pkg.go.dev/github.com/hebcal/hebcal-go)

Hebcal is a perpetual Jewish Calendar. This library converts between
Hebrew and Gregorian dates, and generates lists of Jewish holidays for
any year (past, present or future). Shabbat and holiday candle lighting
and havdalah times are approximated based on location. Torah readings
(Parashat HaShavua), Daf Yomi, and counting of the Omer can also be
specified. Hebcal also includes algorithms to calculate yahrzeits,
birthdays and anniversaries.

Hebcal was created in 1992 by Danny Sadinoff as a Unix/Linux program
written in C, inspired by similar functionality written in Emacs Lisp.
This Golang implementation was released in 2022 by Michael J. Radwin.

Many users of this library will utilize the HebrewCalendar and HDate
interfaces.

Hebcal incorporates and uses several related packages:

  - dailylearning: a plugin registry for daily learning schedules
    (Daf Yomi, Mishna Yomi, etc.). The schedules themselves live in a
    separate module, github.com/hebcal/learning, which registers them
    here; import that module to enable daily learning events.
  - event: an interface for calendar events.
  - greg: converts between Gregorian dates and R.D. (Rata Die)
    day numbers.
  - hdate: converts between Hebrew and Gregorian dates.
    Also includes functions for calculating personal anniversaries
    (Yahrzeit, Birthday) according to the Hebrew calendar.
  - hebcal: provides functionality for calculating Jewish holidays,
    candle-lighting and havdalah times, and fast start/end times.
  - locales: translations and transliterations of Jewish holiday
    names into several languages.
  - molad: calculates the time at which the New Moon is born.
  - omer: calculates the Sefirat HaOmer.
  - sedra: weekly Torah reading (Parashat HaShavua).
  - zmanim: calculates halachic times.
