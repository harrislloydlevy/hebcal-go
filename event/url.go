package event

import (
	"fmt"
	"strconv"
	"strings"
	"time"
)

// URLer is implemented by events that have a canonical page on hebcal.com.
//
// It is deliberately a separate interface rather than a method on CalEvent:
// CalEvent is implemented outside this package, and widening it would break
// those implementations. Callers should use the URL helper below, which
// returns "" for events that do not carry a URL.
type URLer interface {
	// URL returns the absolute hebcal.com URL for this event, or "" if the
	// event has no page (for example a Molad, or a date outside the range the
	// website covers).
	URL() string
}

// URL returns the canonical hebcal.com URL for ev, or "" if it has none.
func URL(ev CalEvent) string {
	if u, ok := ev.(URLer); ok {
		return u.URL()
	}
	return ""
}

// urlFriendly converts a holiday or parsha name into the slug used in
// hebcal.com paths: lower case, apostrophes dropped, spaces hyphenated.
func urlFriendly(s string) string {
	s = strings.ToLower(s)
	s = strings.ReplaceAll(s, "'", "")
	s = strings.ReplaceAll(s, "’", "")
	s = strings.ReplaceAll(s, " ", "-")
	return s
}

// gregYearInRange reports whether a Gregorian year has a page on the website.
func gregYearInRange(gy int) bool {
	return gy >= 100 && gy <= 2999
}

// URL returns the holiday's page on hebcal.com, e.g.
// https://www.hebcal.com/holidays/sukkot-2026 .
//
// The year suffix is not always the event's own Gregorian year:
//
//   - Asara B'Tevet can fall twice in one Gregorian year, so it is
//     disambiguated with a full YYYYMMDD suffix.
//   - Chanukah can start in December and end in January, and the whole
//     festival is filed under the year it began.
//
// Israel-only observances carry ?i=on so the page shows the Israel schedule.
func (ev HolidayEvent) URL() string {
	gy, gm, gd := ev.Date.ProlepticGreg()
	if !gregYearInRange(gy) {
		return ""
	}
	var suffix string
	switch {
	case ev.Desc == "Asara B'Tevet":
		suffix = fmt.Sprintf("%04d%02d%02d", gy, int(gm), gd)
	case strings.HasPrefix(ev.Desc, "Chanukah"):
		year := gy
		if gm == time.January {
			year--
		}
		suffix = strconv.Itoa(year)
	default:
		suffix = strconv.Itoa(gy)
	}
	url := "https://www.hebcal.com/holidays/" + urlFriendly(ev.Basename()) + "-" + suffix
	if ev.Flags&IL_ONLY != 0 {
		url += "?i=on"
	}
	return url
}

// URL returns the parsha's page on hebcal.com, e.g.
// https://www.hebcal.com/sedrot/bereshit-20261010 . The date is the Saturday
// on which the portion is read, which is what makes the URL unique across the
// triennial and doubled-parsha variations.
func (ev parshaEvent) URL() string {
	gy, gm, gd := ev.Date.ProlepticGreg()
	if !gregYearInRange(gy) {
		return ""
	}
	url := "https://www.hebcal.com/sedrot/" +
		urlFriendly(strings.Join(ev.Parsha.Name, "-")) +
		fmt.Sprintf("-%04d%02d%02d", gy, int(gm), gd)
	if ev.IL {
		url += "?i=on"
	}
	return url
}

// URL returns the Hebrew date's converter page, e.g.
// https://www.hebcal.com/converter?hd=15&hm=Nisan&hy=5786&h2g=1 .
func (ev hebrewDateEvent) URL() string {
	hd := ev.Date
	if hd.Year() < 1 {
		return ""
	}
	return fmt.Sprintf("https://www.hebcal.com/converter?hd=%d&hm=%s&hy=%d&h2g=1",
		hd.Day(), hd.MonthName("en"), hd.Year())
}
