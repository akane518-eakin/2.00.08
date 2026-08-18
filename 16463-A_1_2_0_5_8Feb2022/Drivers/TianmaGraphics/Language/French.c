/**********************************************************************************************************
 *  Marturion Electronics Ltd
 *
 *  Knockmore Hill Business Park
 *  9 Ferguson Drive
 *  Lisburn
 *  Co. Antrim
 *  Northern Ireland
 *  BT28 2EX
 *
 *  Copyright 2017, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  French.c
 * Date Created:  Mon 18 Dec 2017 10:02:49 AM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/


/*************************************************************************************************
* Function Name :
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0					W. Paul			Created in English
* 0.2.0		22/11/18	W. Paul			Translate to French Phrases  see email Mon 19Nov2019 Tomas Cooney
*************************************************************************************************/

const char* FrenchPhrase[] =
{
	"English",
	"Française",
	"Deutsch",
	"Español",
	"Netherlandse",
	"Italiano",
	"عربي" ,
        "Suomi",
	"Norsk",
	"Português",
	"ελληνικά",
	"Indonesia",
        "Latviešu",
	"Polski",
	"Rumuński",
        "Svenska",
        "Türkçe",
        "Tiếng Việt",
        
	"Jan",
	"Fév",
	"Mar",
	"Avr",
	"Mai",
	"Juin",
	"Juil",
	"Août",
	"Sept",
	"Oct",
	"Nov",
	"Déc",

	"Dim",
	"Lun",
	"Mar",
	"Mer",
	"Jeu",
	"Ven",
	"Sam",

	"Réglage supprimé",
	"Réussi",
	"Remarque",
	"Avertissement",
	"Défaut",
	"Défaut critique",

	"Mem Rd/Wr",
	"RTC",
	"Débit d'O² d'étalonnage a",
	"Débit d'O² d'étalonnage b",
	"Débit d'O² d'étalonnage c",
	"Débit d'O² d'étalonnage d",
	"Débit d'air d'étalonnage a",
	"Débit d'air d'étalonnage b",
	"Débit d'air d'étalonnage c",
	"Débit d'air d'étalonnage d",
	"Sonde d'O² d'étalonnage",
	"Capteur PP d'étalonnage",
	"SwGenErr",
	"Batterie",
	"5V",
	"24V",
	"Alimentation en air",
	"Alimentation en O²",
	"Alimentation c.a.",
	"Sonde d'O²",
	"PP Défaut de capteur",
	"Contact maintenu",
	"Touche maintenue",
	"Charge de la batterie",
	"Étalonnage d'O²",
	"P Min",
	"P Max",
	"Apnée",
	"FMax",
	"Limite de P",
	"FiO² élevé",
	"FiO² bas",
	"Not Used 32",
	"Not Used 33",
	"Défaut du ventilateur",
	"Not Used 35",
	"Air Défaut de capteur",
	"O² Défaut de capteur",
	"Débit d'O² d'étalonnage e",
	"Débit d'air d'étalonnage e",

	"S/N",
	"Ver:",
	"Résultat d'auto-contrôle",
	"Charge",

	"AIR",
	"DÉBIT",
	"OXYGÈNE",
	"O² Conc",
	" -Max 250",
	"Débit",
	"F-Air",
	" -O²",
	" -Tot",
	"Patient",
	"Pression",
	" - Réglage du débit",
	"% Oxygène",
	"Pression (CPAP) cm H²O",
	"P min",
	"P max",
	"F max",
	" - Réglage du débit - Neutralisation",

	"Cal EN",
	"Disjoncteur",
	"Minuteur",
	"Ventilateur",
	"Audio",
	"Vol+",
	"Vol-",
	"Échelle",
	"COMM TOUS",
	"COMM VENT",
	"DÉBIT D'AIR",
	"DÉBIT O²",
	"Étalonner les capteurs",

	"cmH²O",
	"RR",
	"L/min",
	"hr",
	"min",
	"/min",
	"Jour",
	"Jours",

	"Étalonnage de la sonde d'Oxygène",
	"Voulez-vous étalonner la sonde?",
	"Noter: que ce processus prend 75 s",
	"Étalonnage par touche",
	"POINT",
	"Terminé",
	"Éteindre l'appareil",

	"Oui",
	"Non",
	"Veuillez patienter",
	"Restant",
	"Confirmer l'action",
	"MENU",
	"OK",
	"Bat",
	"En charge",
	"Vol",
	"Oxygène",
	"Désactivé",
	" - Paramètres d'alarme",
	"Désactivé",
	"Accepter",
	"Alarme",
	"DÉMO",

	"ARRÊT",
	"Arrêt",
	"Couper l'alarme du système",
	"Déverrouiller l'écran",
	"Arrêter la thérapie",
	"Confirmer la neutralisation du débit",
	"Quitter la neutralisation du débit",
	"Quitter la neutralisation du débit",
	"Régler la neutralisation du débit",
	"Aucun changement",
	"Confirmer le changement de réglage du débit",
	"Confirmer le changement d'alarme",
	"Confirmer le changement de nébuliseur",
	"L'alimentation secteur est débranchée",
	"Continuer avec l'alimentation par batterie uniquement",
	"Étalonner la sonde d'oxygène maintenant ?",
	"Débit du nébuliseur insuffisant",
	"Augmenter le débit à 10 L/min ?",
	"Erreur d'étalonnage",
	"Remarque : pas d'alimentation en air",
	"Remarque : pas d'alimentation en oxygène",
	"Régler les paramètres de débit ?",
	"Remarque : deux alimentations en gaz disponibles",
	"Niveau de batterie critique",
	"La thérapie s'est arrêtée",
	"Cet appareil va s'arrêter dans",
	"Pas de gaz disponible",

	"Démarrer la thérapie CPAP",
	"Démarrer la thérapie CPAP Paed",
	"Démarrer la thérapie CPAP Helmet",
	"Démarrer la thérapie Bubble PAP",
	"Démarrer la thérapie HFOT",
	"Démarrer la thérapie POINT",
};

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
