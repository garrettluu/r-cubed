/**
 * @file
 * @author Matt "MateoConLechuga" Waltz
 * @brief Contains useful debugging features for use with the integrated CEmu debugger
 */

/**
 * void dbg_sprintf(out, const char*, ...)
 *
 * @brief Use to print to the emulator console.
 * 
 * See the syntax for 'printf' for more information
 * @param out Can be dbgout (black) or dbgerr (red)
 * @def dbg_sprintf
 */

/**
 * void dbg_Debugger(void)
 *
 * @brief Opens the debugger
 * @def dbg_Debugger
 */

/**
 * void dbg_SetBreakpoint(void *address)
 *
 * @brief Sets a breakpoint at a particular address in an emulator
 * @param address The address of the breakpoint to set
 * @def dbg_SetBreakpoint
 */

/**
 * @def dbg_RemoveBreakpoint
 * void dbg_RemoveBreakpoint(void *address)
 *
 * @brief Removes a breakpoint at a particular address in an emulator
 * @param address The address of the breakpoint to remove
 */

/**
 * void dbg_SetWatchpoint(void *address, unsigned length)
 *
 * @brief Sets a non-breaking watchpoint at a particular address in an emulator
 * @param address The address of the watchpoint to set
 * @param length The size of the data at the address (values 1-4)
 * @def dbg_SetWatchpoint
 */

/**
 * void dbg_SetReadWatchpoint(void *address, unsigned length)
 *
 * @brief Sets a read watchpoint at a particular address in an emulator
 * @param address The address of the watchpoint to set
 * @param length The size of the data at the address (values 1-4)
 * @def dbg_SetReadWatchpoint
 */
 
/**
 * void dbg_SetWriteWatchpoint(void *address, unsigned length)
 *
 * @brief Sets a write watchpoint at a particular address in an emulator
 * @param address The address of the watchpoint to set
 * @param length The size of the data at the address (values 1-4)
 * @def dbg_SetWriteWatchpoint
 */

/**
 * void dbg_SetReadWriteWatchpoint(void *address, unsigned length)
 *
 * @brief Sets a read and write watchpoint at a particular address in an emulator
 * @param address The address of the watchpoint to set
 * @param length The size of the data at the address (values 1-4)
 * @def dbg_SetReadWriteWatchpoint
 */

/**
 * void dbg_RemoveWatchpoint(void *address)
 *
 * @brief Removes a watchpoint at a particular address in an emulator
 * @param address The address of the watchpoint to remove
 * @def dbg_RemoveWatchpoint
 */
 
/**
 * void dbg_RemoveAllBreakpoints(void)
 * @brief Removes all breakpoints in an emulator
 * @def dbg_RemoveAllBreakpoints(void)
 */
 
/**
 * void dbg_RemoveAllWatchpoints(void)
 * @brief Removes all watchpoints in an emulator
 * @def dbg_RemoveAllWatchpoints
 */

/* Preprocessor definitions (should not be looked at :P) */
#ifdef dbg_Debugger
#undef dbg_Debugger
#endif

#ifdef dbg_SetBreakpoint
#undef dbg_SetBreakpoint
#endif

#ifdef dbg_RemoveBreakpoint
#undef dbg_RemoveBreakpoint
#endif

#ifdef dbg_RemoveWatchpoint
#undef dbg_RemoveWatchpoint
#endif

#ifdef dbg_SetWatchpoint
#undef dbg_SetWatchpoint
#endif

#ifdef dbg_SetReadWriteWatchpoint
#undef dbg_SetReadWriteWatchpoint
#endif

#ifdef dbg_SetReadWatchpoint
#undef dbg_SetReadWatchpoint
#endif

#ifdef dbg_SetWriteWatchpoint
#undef dbg_SetWriteWatchpoint
#endif

#ifdef dbg_RemoveAllWatchpoints
#undef dbg_RemoveAllWatchpoints
#endif

#ifdef dbg_RemoveAllBreakpoints
#undef dbg_RemoveAllBreakpoints
#endif

#ifdef dbg_sprintf
#undef dbg_sprintf
#endif

#ifdef dbgout
#undef dbgout
#endif

#ifdef dbgerr
#undef dbgerr
#endif

#ifndef NDEBUG
#define dbg_Debugger Debugger
#define dbg_SetBreakpoint SetBreakpoint
#define dbg_RemoveBreakpoint RemoveBreakpoint
#define dbg_SetWatchpoint SetWatchpoint
#define dbg_SetReadWatchpoint SetReadWatchpoint
#define dbg_SetWriteWatchpoint SetWriteWatchpoint
#define dbg_SetReadWriteWatchpoint SetReadWriteWatchpoint
#define dbg_RemoveWatchpoint RemoveWatchpoint
#define dbg_RemoveAllWatchpoints RemoveAllWatchpoints
#define dbg_RemoveAllBreakpoints RemoveAllBreakpoints
#define dbg_sprintf sprintf
#define dbgout ((const char*)0xFB0000)
#define dbgerr ((const char*)0xFC0000)
#else
#define dbg_Debugger(ignore)                      //
#define dbg_SetBreakpoint(ignore)                 //
#define dbg_RemoveBreakpoint(ignore)              //
#define dbg_SetWatchpoint(ignore,ignore)          //
#define dbg_SetReadWatchpoint(ignore,ignore)      //
#define dbg_SetWriteWatchpoint(ignore,ignore)     //
#define dbg_SetReadWriteWatchpoint(ignore,ignore) //
#define dbg_RemoveWatchpoint(ignore)              //
#define dbg_RemoveAllWatchpoints(ignore)          //
#define dbg_RemoveAllBreakpoints(ignore)          //
#define dbg_sprintf                               //
#define dbgout (NULL)
#define dbgerr (NULL)
#endif

/* simple function prototypes (should not ever be used or even looked at) */
void Debugger(void);
void SetBreakpoint(void *address);
void RemoveBreakpoint(void *address);
void SetWatchpoint(void *address, unsigned length);
void SetReadWatchpoint(void *address, unsigned length);
void SetWriteWatchpoint(void *address, unsigned length);
void SetReadWriteWatchpoint(void *address, unsigned length);
void RemoveWatchpoint(void *address);
void RemoveAllWatchpoints(void);
void RemoveAllBreakpoints(void);

