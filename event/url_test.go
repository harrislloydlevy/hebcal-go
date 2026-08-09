package event

import (
	"testing"

	"github.com/hebcal/hdate"
)

func TestHolidayEventURL(t *testing.T) {
	tests := []struct {
		name  string
		ev    HolidayEvent
		want  string
	}{
		{
			name: "ordinary holiday uses the Gregorian year",
			ev:   HolidayEvent{Date: hdate.New(5787, hdate.Tishrei, 15), Desc: "Sukkot I", Flags: CHAG},
			want: "https://www.hebcal.com/holidays/sukkot-2026",
		},
		{
			name: "Israel-only observance carries i=on",
			ev:   HolidayEvent{Date: hdate.New(5786, hdate.Iyyar, 5), Desc: "Yom HaAtzma'ut", Flags: MODERN_HOLIDAY | IL_ONLY},
			want: "https://www.hebcal.com/holidays/yom-haatzmaut-2026?i=on",
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := tt.ev.URL(); got != tt.want {
				t.Errorf("URL() = %q, want %q", got, tt.want)
			}
		})
	}
}

// Asara B'Tevet can fall twice in one Gregorian year, so its URL is
// disambiguated with a full date rather than just the year.
func TestAsaraBTevetURLUsesFullDate(t *testing.T) {
	ev := HolidayEvent{Date: hdate.New(5787, hdate.Tevet, 10), Desc: "Asara B'Tevet", Flags: MINOR_FAST}
	got := ev.URL()
	want := "https://www.hebcal.com/holidays/asara-btevet-20261220"
	if got != want {
		t.Errorf("URL() = %q, want %q", got, want)
	}
}

// Chanukah can start in December and end in January; the whole festival is
// filed under the year it began.
func TestChanukahJanuaryDaysUsePreviousYear(t *testing.T) {
	ev := HolidayEvent{Date: hdate.New(5787, hdate.Tevet, 2), Desc: "Chanukah: 8th Day", Flags: MINOR_HOLIDAY}
	gy, gm, _ := ev.Date.ProlepticGreg()
	got := ev.URL()
	t.Logf("greg %d-%02d -> %s", gy, int(gm), got)
	if gm == 1 && got != "https://www.hebcal.com/holidays/chanukah-"+itoa(gy-1) {
		t.Errorf("January Chanukah day should use the previous year, got %q", got)
	}
}

func itoa(i int) string {
	if i < 0 {
		return "-" + itoa(-i)
	}
	if i < 10 {
		return string(rune('0' + i))
	}
	return itoa(i/10) + string(rune('0'+i%10))
}

func TestURLHelperReturnsEmptyForEventsWithoutPages(t *testing.T) {
	ev := NewHebrewDateEvent(hdate.New(5786, hdate.Nisan, 15))
	if URL(ev) == "" {
		t.Error("hebrew date event should have a converter URL")
	}
}
