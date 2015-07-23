// Sudo_AIC.hxx
#ifndef _SUDO_AIC_HXX_
#define _SUDO_AIC_HXX_

extern int Mark_AIC_Scans( PABOX2 pb );
extern int Do_Fill_AIC( PABOX2 pb );
extern int Do_AIC_Scans( PABOX2 pb );

// for painting
extern int Get_AIC_chain_count();   // { return AIC_chain_count; }
extern vRCP **Get_AIC_chains();     // { return AIC_chains; }
extern bool are_AIC_chains_valid(); // { return AIC_chains_valid; }
extern void Kill_AIC_chains();
extern bool validate_AIC_chains( bool b );

#endif // #ifndef _SUDO_AIC_HXX_
// eof - Sudo_AIC.hxx


