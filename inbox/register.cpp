
#include <cstdint>
#include <stdio.h>

using std::uint32_t;

// I need to consider how I could switch out "real" register classes (for use on an embedded device) for a "fake" class that can be used in unit testing, host-compiled binaries, etc. - these could be used to tie into a simulated address space, print debugging messages etc. Are templates a way to do this? Or just a whole "fake register" class template?

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
		// (*reinterpret_cast<pointer>(address)) = val;
		printf( "Write to 0x%08X with value 0x%08x\n", address, val );
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
// // or:
// PINA.SetBit( PINA::PIN7 )
// // or:
// PINA.SetBit( PIN7 )
// // Since it's probably unnecessary to declare PIN<BANK><NUMBER> for all BANKs
// 
// Maybe there's something clever (oh shit, there it is again) by defining a more complex type for the masks, another template or something, so that you can specify the field (multi-bit value) and the value to assign and it will handle the shift and mask operation, type checking, etc.
// 
// It's also important to consider that these register offsets are often multi-level - for example, consider this define set
// 
// #define GPIO_PORT_A_BASE		(PERIPH_BASE_AHB1 + 0x0000)
// #define PERIPH_BASE			(0x40000000U)
// #define PERIPH_BASE_AHB1		(PERIPH_BASE + 0x20000)
// 
// So it might be good to be able to build register sets upon one another. Maybe have a RegisterBase type that just has an address but no actions; if you build upon it your register adds its offset to it to calculate the address. I'm not actually sure how to structure that but it seems cleaner than hiding it in a bunch of defines. The better option, IMO, would be to use the exact peripheral offset instead.
// 
// RegisterBase<uint32_t,0x40000000U> PERIPH_BASE;
// RegisterBase<PERIPH_BASE,0x20000> PERIPH_BASE_AHB1;
// RegisterBase<PERIPH_BASE_AHB1,0x0000> GPIO_PORT_A_BASE;
// Register<GPIO_PORT_A_BASE,0x0> GPIOA_MODER;
// 
// I guess in this case the children would inherit the register width? I'm not sure if it makes sense to have the register width in the parent or not.
// This could potentially even be used to enforce access types (e.g. "device" and "strongly ordered" types, insert memory barriers, etc.).
// 
// Is there a way to use this class in a slightly more generic way? Say you want to provide a "SPI" register template that can apply to SPI[1:6] but the specific instance is stored in a pointer instead of access directly?
// This way lines like:
// 
// setbits_le32(priv->base + STM32_SPI_IFCR, SPI_IFCR_ALL);
// 
// Can become:
// 
// SPI_IFCR.SetBits( priv->base, SPI_IFCR_ALL )
// 
// (or some cleaner looking version where you don't need the `priv->base` in each function somehow)

// I think the goal of the "pointer-based" solution would be to still compile things like:
// 
// // Declared: priv = Register(pointer);
// priv.REG.SetBit( BIT )
// 
// To:
// 
// *(base + offset) |= BIT;
// 
// Not to a function call that does that. The question is how to do that...
// Like, if the template class is all static methods and the only variable it stores is the base address (as a value or a pointer) then in theory I think the methods should all get inlined...

template<typename T>
struct RegisterRef {
	RegisterRef( T addr ) : addr_(addr) {}

	typedef volatile T * pointer;

	// TODO: Probably need a version without const?
	// TODO: Replace with typedef for const value
	void operator= ( const T val )
	{
		// TODO: Handle conversion of different size data (e.g. uint8_t) to the register width (?)
		// TODO: Is there a way to do this same operation, like, a lot? I want to override all operators on this value (e.g. |=, &=, etc.) and this just seems obnoxious
		(*reinterpret_cast<pointer>(addr_)) = val;
	}

	// Mkay, what other operations do we need?
	// - Return value (other side of assignment)
	// - Other operator overloads: |= &= += -= == (?)
	// - Could provide facilities for access permissions (like that one paper) - e.g. read only, write only, read/write
	// - Could also use these same facilities to provide proper handling of "unused" bits of a register (e.g. where they need to always be set to 1 or 0); this has some static and dynamic cost implications (const input value + const mask = compile-time calculation; non-const input (e.g. variable) + const mask = dynamic calculation)
	// bit_set, bit_clear, bit_flip, bit_check
private:
	T addr_;
};

static void do_stuff( RegisterRef<uint32_t> & ref )
{
	// RegisterRef<uint32_t> REF_CM_PER_GPIO1_CLKCTRL = RegisterRef<uint32_t>(base);
	// REF_CM_PER_GPIO1_CLKCTRL = 0x40002;
	ref = 0x40002;
}

void main()
{
	// Absolute register reference (static)
	// Register<uint32_t,0x44e000AC> CM_PER_GPIO1_CLKCTRL;
	// CM_PER_GPIO1_CLKCTRL = 0x40002;

	// Pointer-based register reference (variable-based address)
	// uint32_t base = 0x44e000AC;
	// RegisterRef<uint32_t> REF_CM_PER_GPIO1_CLKCTRL = RegisterRef<uint32_t>(base);
	// REF_CM_PER_GPIO1_CLKCTRL = 0x40002;
	
	RegisterRef<uint32_t> REF_CM_PER_GPIO1_CLKCTRL_1 = RegisterRef<uint32_t>(0x44e000AC);
	RegisterRef<uint32_t> REF_CM_PER_GPIO1_CLKCTRL_2 = RegisterRef<uint32_t>(0x44e000BC);

	do_stuff( REF_CM_PER_GPIO1_CLKCTRL_1 );
	do_stuff( REF_CM_PER_GPIO1_CLKCTRL_2 );

	while(1);
}

// Is there a way to declare registers like I have above but *also* be able to reference them via pointers / references?
//
// For example, I'd like to do:
// 
// Register<uint32_t,ADDRESS> REG_INSTANCE;
// 
// And then be able to do:
// 
// Register<uint32_t> *inst = REG_INSTANCE;
// *inst = VALUE;
// 
// Or is it better to do stuff like pass around register base addresses (or the offset or something through a template) and inside of functions do stuff like:
// 
// void device::do_something( device_t * priv )
// {
// 	Register<uint32_t,priv->base_addr> inst;
// 	inst = VALUE;
// }
