<?php
/**
 * The base configuration for WordPress
 *
 * The wp-config.php creation script uses this file during the installation.
 * You don't have to use the web site, you can copy this file to "wp-config.php"
 * and fill in the values.
 *
 * This file contains the following configurations:
 *
 * * Database settings
 * * Secret keys
 * * Database table prefix
 * * Localized language
 * * ABSPATH
 *
 * @link https://wordpress.org/support/article/editing-wp-config-php/
 *
 * @package WordPress
 */

// ** Database settings - You can get this info from your web host ** //
/** The name of the database for WordPress */
define( 'DB_NAME', 'Database_MariaBd' );

/** Database username */
define( 'DB_USER', 'afadlane' );

/** Database password */
define( 'DB_PASSWORD', '1234' );

/** Database hostname */
define( 'DB_HOST', 'mariadb:3306' );

/** Database charset to use in creating database tables. */
define( 'DB_CHARSET', 'utf8' );

/** The database collate type. Don't change this if in doubt. */
define( 'DB_COLLATE', '' );

/**#@+
 * Authentication unique keys and salts.
 *
 * Change these to different unique phrases! You can generate these using
 * the {@link https://api.wordpress.org/secret-key/1.1/salt/ WordPress.org secret-key service}.
 *
 * You can change these at any point in time to invalidate all existing cookies.
 * This will force all users to have to log in again.
 *
 * @since 2.6.0
 */
define( 'AUTH_KEY',          'PI^z6tsM=0Xxag?&$-k^7})_W6n>aRomi]A(&j#o0{(wI[cvT-wPLhGL%!<V&HWl' );
define( 'SECURE_AUTH_KEY',   '-[R#L_P_xv5n7xUQ[Ev[`OnO4V&b/5TTslpz]2F)N@<;4aUc?>=<p {^B=:jV9) ' );
define( 'LOGGED_IN_KEY',     '[~I=kJx;Z||k;3.F2/;P(`nSx+:cwWy,3-*@pd9RN/NQ ^8+u -|oHV&p8~s%)(^' );
define( 'NONCE_KEY',         'S9TQtho;&#?9<4qp99,961fcM(vBZU0W|>?(F`b #CjQVh9+mO`q0+8y9}ZC;SV#' );
define( 'AUTH_SALT',         'DK_2m-9.>w#y0bN7iTgCTG$jC]xN^2*[&?X%#S29;bNKqGE $ZuC#Fu#dCa6;y+N' );
define( 'SECURE_AUTH_SALT',  'RnYAETe]n~[|A8Y1N][w}E=1>ss~Bp5(]U_WPHbabf7kF6pZj&2+&,Eo7lbzM$)!' );
define( 'LOGGED_IN_SALT',    '=blC~xGh6]He=:)+ROw_RK@FrT]*iYmMq,eY7hKxFY|10S6C^g#f,FvZ $YF,|Ae' );
define( 'NONCE_SALT',        '=n`tG2hM8@%hs{pcMow{72+bm_p%}^%C&80O]yqnlHU]&@gK0&!YG8H3(7sE3{jI' );
define( 'WP_CACHE_KEY_SALT', '=2,Qjxx!f{&B8+FeY<F)<B)dnLI(9DJgqy4^1XuYdmU^2@!Umh}MkI%!MVvJ{Q+/' );


/**#@-*/

/**
 * WordPress database table prefix.
 *
 * You can have multiple installations in one database if you give each
 * a unique prefix. Only numbers, letters, and underscores please!
 */
$table_prefix = 'wp_';


/* Add any custom values between this line and the "stop editing" line. */



/**
 * For developers: WordPress debugging mode.
 *
 * Change this to true to enable the display of notices during development.
 * It is strongly recommended that plugin and theme developers use WP_DEBUG
 * in their development environments.
 *
 * For information on other constants that can be used for debugging,
 * visit the documentation.
 *
 * @link https://wordpress.org/support/article/debugging-in-wordpress/
 */
if ( ! defined( 'WP_DEBUG' ) ) {
	define( 'WP_DEBUG', false );
}

/* That's all, stop editing! Happy publishing. */

/** Absolute path to the WordPress directory. */
if ( ! defined( 'ABSPATH' ) ) {
	define( 'ABSPATH', __DIR__ . '/' );
}

/** Sets up WordPress vars and included files. */
require_once ABSPATH . 'wp-settings.php';
