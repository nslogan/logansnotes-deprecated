
#include <cstdint>

using std::uint32_t;

// Hmm. Which constant type is correct for the address? Maybe I use the type T (?). Or maybe size_t (?). This should always (?) be the size of the memory architecture (I think) - so 8 (AVR) or [32,64] (ARM)
// TODO: Better name than `address`
template<typename T, T address>
struct Register {
	// Convenience types
	// typedef _Tp                                        value_type;
 //    typedef value_type*                                pointer;
 //    typedef const value_type*                        const_pointer;
 //    typedef value_type&                                reference;
 //    typedef const value_type&                        const_reference;
 //    typedef value_type*                                iterator;
 //    typedef const value_type*                        const_iterator;
 //    typedef size_t                                    size_type;

	typedef volatile T * pointer;

	// TODO: Probably need a version without const?
	// TODO: Replace with typedef for const value
	void operator= ( const T val )
	{
		// TODO: Handle conversion of different size data (e.g. uint8_t) to the register width (?)
		// TODO: Is there a way to do this same operation, like, a lot? I want to override all operators on this value (e.g. |=, &=, etc.) and this just seems obnoxious
		(*reinterpret_cast<pointer>(address)) = val;
	}

	// Mkay, what other operations do we need?
	// - Return value (other side of assignment)
	// - Other operator overloads: |= &= += -= == (?)
	// - Could provide facilities for access permissions (like that one paper) - e.g. read only, write only, read/write
	// - Could also use these same facilities to provide proper handling of "unused" bits of a register (e.g. where they need to always be set to 1 or 0); this has some static and dynamic cost implications (const input value + const mask = compile-time calculation; non-const input (e.g. variable) + const mask = dynamic calculation)
	// bit_set, bit_clear, bit_flip, bit_check
};

// TODO: Some convenient way of defining a Register template with the type already filled in (e.g. uint32_t). Maybe I namespace my template and then the local developer defines their own version with the correct type?
// Or maybe I detect the type from the environment somehow? Like via the size of a pointer?
// Or I could make some pre-defined ones like Reg64, Reg32, Reg8, etc.

// Is there some way to use this same sort of concept but to build a more complex structure? Like, for the UART module I want to be able to access all of the registers and I have the base address plus offsets. Seems like a good application of this sort of thing.

// Is there something clever (oh no, don't be clever) I can do with regards to bit definitions? Like, it'd be neat to define the register plus it's bits - that automatically scopes those values. Maybe an enum? Hmm.
// One challenging part of this is multi-bit values (so start bit + width = mask and shift operation)

// So how would I scope this enum *into* the PINA struct definition? Is there a convenient syntax?
// enum {
// 	PINA7 = 7,
// 	PINA6 = 6,
// 	PINA5 = 5,
// 	PINA4 = 4,
// 	PINA3 = 3,
// 	PINA2 = 2,
// 	PINA1 = 1,
// 	PINA0 = 0
// };
// Reg8<0x00> PINA;
// 
// // But this would be dope:
// PINA.SetBit( PINA::PINA7 );
// 
// Maybe there's something clever (oh shit, there it is again) by defining a more complex type for the masks, another template or something, so that you can specify the field (multi-bit value) and the value to assign and it will handle the shift and mask operation, type checking, etc.

int main()
{
	Register<uint32_t,0x44e000AC> CM_PER_GPIO1_CLKCTRL;

	CM_PER_GPIO1_CLKCTRL = 0x40002;
}
