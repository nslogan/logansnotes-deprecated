# Register Tool

I want a tool that:

- Documents register layouts
- Can convert a register value into a visual representation of that value laid out on the register map
	+ What bits are set
	+ What values are associated with those bits being set
- Can be used to generate code (or at the format the register map is stored in can be used)
- Can calculate register addresses by combining the offset and base address (including if there are multiple peripherals with different base addresses)
- Can cross-reference other associated registers (e.g. SDMMC_CLKCR could reference the RCC for its base clock)
- Stores all associated information with a register
	+ Register name
	+ Offset from peripheral base
	+ Peripheral base (registers reference parent(s))
	+ Reset values
	+ Access semantics - e.g. only valid when this clock enabled (associated register)
		* Different width access may cause different behavior (e.g. 8- vs 16- vs 32-bit read / write)
	+ Bit access type (r, rw, w, reserved)
		* Also good to capture semantics like "write to clear"
	+ Field naming
	+ Field meaning, including enums
	+ Notes (e.g. "At least seven PCLK2 clock periods are needed between two write accesses to this register.")
	+ Any additional useful meta-data
		* Associated bus used to access register (e.g. APB2)
	+ Datasheet section, page, etc.
	+ Associated registers - e.g. "status flags" and "clear status flags"
- Searchable - I want to be able to quickly find a register by peripheral, name, etc. Like STs fuzzy search.
- Tabs - have multiple register definitions open at the same time

## Storage Format

JSON, YAML, XML (no), database

- A peripheral has registers
- There can be multiple peripherals that share the same register layout
	+ There could be slight differences between peripheral instances so there should be some sort of inheritance with the ability to override
- A register
	+ Has a name
	+ Has a description
	+ Has a width
	+ Has an address offset (and a register "instance" has an actual address)
	+ Has fields
	+ Has access semantics
- A field
	+ Has a name
	+ Has a description
	+ Is one or more bits in a register
	+ Has an associated value description - e.g. "clock divide factor" or enum


```json
{
	[
		"name" : "SDMMC_CLKCR",
		"long_name" : "SDMMC clock control register",
		"addr_offset" : "0x04",
		"reset_value" : "0x00000000"
		"description" : "The SDMMC_CLKCR register controls the SDMMC_CK output clock.",
		"section" : "39.8.2"
		"fields" : [
			{
				"name" : "CLKDIV",
				"long_name" : "Clock divide factor",
				"description" : "This field defines the divide factor between the input clock (SDMMCCLK) and the outputclock (SDMMC_CK): SDMMC_CK frequency = SDMMCCLK / [CLKDIV + 2].",
				"start_bit" : 0,
				"end_bit" : 7,
				"access" : "rw"
			}
		]
	]
}
```

### Register Address Calculation

Peripheral Base + Offset
Parent Tree Base + Offset

Offset can be "offset + instance * factor" for registers like SDMMC_RESPx


