

// Mux function

// ADG732 (32-channel)
// 
// EN       Enable; on test EN is tied to GND (always enabled)
// A[0:3]   Address 
// CS_n     Chip Select (active low)
// WR_n     Write (active low)
// S[16:1]  Source (input)
// D        Drain (output)
// 
// Functions:
// Set address
//   - Set address pins
//   - Take CS and WR low
//   - Take CS and WR high
// Enable output (not possible with EN tied low)
// 
// Timing information:
// CS to WR (setup) and WR to CS (hold): 0 ns
// WR width: 10 ns (min)
// Time between WR cycles:> 10 ns (min)
// Addr to WR de-assert (setup): 5 ns (min)
// Addr hold: 2 ns (min)
// 
// clk = 16 MHz = 62.5 ns

namespace adg732 {

template<

// template<uint16_t DDR, uint16_t PORT, uint16_t PIN, uint8_t bit, bool polarity>

// The templating for this is interesting - do you do a whole port and have
// functions to set specific bits or do you do a specific bit? I guess it
// depends on use case.
//
// For example, if your GPIO is fixed (like the BMS) then a mapping of 1:1 for
// pins to templates probably makes sense. The pin is known at compile time,
// each one is likely to be used in a different module (or groups of them in
// certain modules), and the direction and polarity aren't changing.
//
// However, for a more general case (like the top board mux), the design
// benefits from having a more flexible template that allows changing
// direction...hmm. Now that I'm thinking about it I'm not as sure. I think
// there's a case to be made for a collection of pins in a port being grouped
// together efficiently (for example, the mux address pins) but I think actually
// the bottom board tester is a better example of a per-port template case. So
// there's probably two classes and one of them has two sub-classes.
//
// 1. Full port
// 2. Sub-port
//    - Single pin
//    - Group pin
//
// I think there's also a case for specialization in terms of input-only,
// output-only, and input-output classes.
//
// Ultimately I think my goal is design an extremely flexible GPIO template
// library that has lots of partial-specializations and class options to fit
// most of the cases I think are useful and common.
//
// Coming back to this after the weekend. There are a couple use cases that are
// common for me. One is a single pin - e.g. "chip select" - and I want to be
// able to easily perform operations like set, clear, flip, check. The other
// case is a register upon which I want to perform operations like direct
// assignment or checking, set mask, clear mask, check mask (maybe), and
// probably even set specific pins. This can get pretty cool (and complicated)
// if I namespace the register bit definitions within its class, so you could do
// stuff like reg.bit.clear() or reg.clear(bit) or something.
// 
// The case I care about right now for this project is single bit and multiple bit GPIO definitions.
//
// template<typename T_reg, T_reg ddr, T_reg port, T_reg pin, uint8_t bit,
// uint8_t polarity> template<typename T_reg, T_reg ddr, T_reg port, T_reg pin>
//
struct gpio_reg_t {
	reg8_t ddr;
	reg8_t port;
	reg8_t pin;
	uint8_t bit;
	uint8_t polarity;
};

// Structure containing references to all input and output pins for mux
struct io_t {
	// en
	cs;
	wr;
	addr[4];
};


void set_address( io_t & mux, uint8_t addr )
{
	// Convenience assignments
	type & cs = mux.cs;
	type & wr = mux.wr;
	type & addr = mux.addr;

	// Select chip and set into write mode
	cs.Set();
	wr.Set();

	// Set address bits
	if ( addr & 0x1 ) { addr[0].Set(); } else { addr[0].Clear(); }
	if ( addr & 0x2 ) { addr[1].Set(); } else { addr[1].Clear(); }
	if ( addr & 0x4 ) { addr[2].Set(); } else { addr[2].Clear(); }
	if ( addr & 0x8 ) { addr[3].Set(); } else { addr[3].Clear(); }

	// Deselect chip, latches address
	cs.Clear();
	wr.Clear();
}

} // namespace adg732
