
//
// Un exemple de boucle avec
// évolution de R1 par incrément de 2000.
//

	define INCR 2000
	define SYSC_EXIT 100
	define SYSC_PUTI 200
	define SYS_NEW_THREAD  300

	
	// *** créer un thread ***
    sysc R1, SYS_NEW_THREAD    // créer un thread
    set R3, 0                   // R3 = 0
    ifgt R1, R3, pere           // si (R1 > R3), aller à pere 

    // *** code du fils ***
    set R3, 1000                // R3 = 1000    
    sysc R3, SYSC_PUTI          // afficher R3  
    nop
    nop  

pere: // *** code du père ***
    set R3, 2000                // R3 = 1000      
    sysc R3, SYSC_PUTI          // afficher R3    
    sysc SYSC_EXIT              // fin du thread  