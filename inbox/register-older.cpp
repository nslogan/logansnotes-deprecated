
#include <iostream>
#include <iomanip>

#include "../src/include/bitops.h"

#include <cstdint>

using std::uint8_t;

typedef void (OnWriteHook_t)( const uint8_t new_value );

// void default_OnWriteHook( const uint8_t new_value ) {}

class Register
{
public:
	Register() : value_( 0x00 ), onWriteHook_( nullptr ) {}
	// Register( const uint8_t reset_val );

	// Mock entry points
	void onWrite( OnWriteHook_t *fn );
	// void onRead(  );

	// Overloaded operators - behave like a normal register
	Register& operator= ( const uint8_t val );
	Register& operator&= ( const uint8_t val );
	Register& operator|= ( const uint8_t val );
	// ++
	// --
	// +=
	// -=
	// <<
	// >>

	// Assign to uint8_t
	operator uint8_t() const;
private:
	uint8_t value_;
	// uint8_t reset_val_;

	// address (?)

	// Hooks
	OnWriteHook_t *onWriteHook_;

	// Helper functions
	void callOnWrite( const uint8_t new_value )
	{
		if ( onWriteHook_ ) { (*onWriteHook_)( new_value ); }
	}
};

Register& Register::operator= ( const uint8_t val )
{
	value_ = val;
	callOnWrite( val );

	return *this;
}

Register& Register::operator&= ( const uint8_t val )
{
	value_ &= val;
	callOnWrite( val );

	return *this;
}

Register& Register::operator|= ( const uint8_t val )
{
	value_ |= val;
	callOnWrite( val );

	return *this;
}

Register::operator uint8_t() const
{
	return value_;
}

void Register::onWrite( OnWriteHook_t * fn )
{
	onWriteHook_ = fn;
}

void PrintSREG( const uint8_t sreg )
{
	// std::cout << "SREG: " << std::to_string(sreg) << std::endl;
	std::cout << "SREG: 0x" << std::hex << std::setfill('0') << std::setw(2)
		<< std::uppercase << static_cast<unsigned int>(sreg) << std::endl;
}


// <avr/common.h>
Register SREG;

// <avr/io.h>
Register UDR0_;

// Cool idea: Redefine the ISR macro to automatically add the interrupt to the
// correct class based on the interrupt specified. For example, configuring your
// ISR with `ISR(USART0_RX_vect)` would perform a lookup in a vector table (set
// up in the library) based on the vector number (in this case, 25 for the
// ATmega2560) and then associate the function with the class. This change adds
// a step to the current configuration (which creates a function based on the
// vector name).
//
// This means I need a class for the part (which I'll have anyway) that I
// associate the peripherals with (and their vectors). I could make this
// semi-automatic based on some design pattern.

// Some things to ponder
//
// When are the ISRs called? Should it behave based on actual interrupt
// semantics? For example, the rx_vect should be called as soon as a write
// occurs to UDR0.
//
// Oh snap. That's an interesting thing about the registers. The UDR0 behaves
// differently based on what's happening. For example, when you write to UDR0 it
// performs a send, but if you read from it reads the TX buffer and clears the
// flag associated with a received byte. Effectively, there are *two* stored
// values for a single Register object - one for the RX side and one for the TX
// side. The internal device side of this is something that the Uart class below
// would need access to, to be able to simulate the receive and transmit
// semantics properly.
//
// Also, I need to consider how to control the flags in the UART status
// registers. For example, the empty/full flags, overrun, etc.
//
// But back to the original question, what kind of interface do I need to
// provide to the test harnessing, and how automatic should it be? I think that
// when you write to UDR0, if interrupts are enabled, the interrupt should fire
// and perform a send. I could provide logging of these sorts of events, or even
// allow additional functionality to be added (custom hooks).
//
// I also need to provide the other side of the equation. For example, a way to
// send a byte to the device, triggering the UDRE vector. I could even provide a
// way to perform interesting things like loopback.
class Uart
{
public:
	Uart( Register udr ) : UDR_( udr )
	{
		Init();
	}

	void Init()
	{
		UDR_ = 0x00;
	}

	// Functions to assign function pointer to vectors

private:
	// Global registers
	Register &SREG_ = SREG;

	// Peripheral-specific registers
	Register &UDR_;

	// Interrupt function pointers
	void (* rx_vect)( void );
	void (* tx_vect)( void );
	void (* udre_vect)( void );
};_


int main()
{
	SREG.onWrite( PrintSREG );

	SREG = 0x14;

	BIT_SET( SREG, 1 );
	BIT_CLEAR( SREG, 1 );

	uint8_t val = SREG;
	std::cout << std::to_string(val) << std::endl;

	Uart Uart0( UDR0_ );

	return 0;
}
