OBSERVACIONES:
	1. En algunas ocasiones, al hacer ctrl+Z, por alguna razón, se imprime dos veces (duplicado) el prompt del minishell.
	Esto no causa ningún error, sencillamente hace que la "simulación" de la consola no sea 100% realista por así decirlo.
	Suponemos que debe ser algún error de compatibilidad.

	2. Al llamar a la función kill, muchas veces salía un warning que advertía sobre que esto podría llevar a errores.
	Investigué un poco en Internet y encontré que si se añade "#define _POSIX_SOURCE" este warning desaparece.
	Al añadirlo, efectivamente, el warning ya no aparece, y al ejecutar el shell final, el funcionamiento parece ser
	el adecuado. De todas formas, si este 'define' supone un problema, se puede quitar sin que afecte a la ejecución
	del programa final; simplemente, aparecerá un warning que avisará sobre este riesgo al llamar a kill.

MEJORAS:
	Hemos añadido una variable general que es un 'char mensaje[2500]' en todos los niveles en los que son necesarios
	comentarios de debugging, para así hacer sprintf de los errores

MIEMBROS DEL GRUPO:
	Mario Ventura Burgos, Luis Miguel Vargas Durán, Felip Antoni Font Vicens