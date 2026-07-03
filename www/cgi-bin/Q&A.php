#!/usr/bin/php-cgi
<?php // Session directory creation
$dir = __DIR__ . '/sessions';
if (!is_dir($dir))
	mkdir($dir, 0733, true);
session_save_path($dir);
?>

<?php // Ini_set for non-restricted use
ini_set('session.use_strict_mode', 0);
ini_set('session.use_cookies', 0);
?>

<?php // C++ session_id extraction
$cpp_sid = preg_replace('/[^a-f0-9]/i', '', $_COOKIE['session_id'] ?? '');
?>

<?php // Session file creation
$sess_file = $dir . '/sess_' . $cpp_sid;
if (!file_exists($sess_file)) {
    file_put_contents($sess_file, '');
    chmod($sess_file, 0666);
}
?>

<?php // Session Id initialization based on C++ session Id
session_id($cpp_sid);
?>

<?php // $_POST parsing forced before session_start()
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $raw_input = file_get_contents('php://input');

    parse_str($raw_input, $_POST);
}
?>

<?php // Session start
session_start();
?>

<?php // Functions
function validateInput(string $input, int $minLen, int $maxLen) : bool {
    $len = strlen($input);
    if ($len < $minLen || $len > $maxLen) {
        return (false);
    }
    
    if (!preg_match('/^[a-zA-Z0-9_-]+$/', $input)) {
        return (false);
    }

    if (stripos($input, '<script') != false
        || stripos($input, 'union') != false
        || stripos($input, 'select') != false)
    {
        return (false);
    }

    return (true);
}
?>

<?php // Session varaibles initialization
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    foreach(['q1', 'q2', 'q3', 'q4', 'q5', 'q6', 'q7', 'q8'] as $q) {
        $answer = trim($_POST[$q] ?? '');
        if (!empty($answer)) {
            if (!validateInput($answer, 1, 100)) {
                $_SESSION[$q] = 'Invalid Input';
            }
            else {
                $_SESSION[$q] = $answer;
            }
        }
    }
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Webserv 42 - Dashboard d'Évaluation</title>
    <style>
        :root {
            --primary: #4f46e5;
            --primary-hover: #4338ca;
            --bg: #f3f4f6;
            --card-bg: #ffffff;
            --text-main: #1f2937;
            --text-muted: #6b7280;
            --border: #e5e7eb;
            --danger: #ef4444;
            --warning: #f59e0b;
            --info: #3b82f6;
            --success: #10b981;
        }

        body {
            font-family: 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
            background-color: var(--bg);
            color: var(--text-main);
            margin: 0;
            padding: 0; /* Padding retiré pour le bandeau */
            display: flex;
            flex-direction: column;
            align-items: center;
            padding-top: 80px; /* Espace pour ne pas cacher le contenu sous le bandeau */
        }

        /* --- BANDEAU CORRECTEUR --- */
        .evaluator-banner {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            background-color: #1e293b;
            color: white;
            padding: 15px 20px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.2);
            z-index: 1000;
            display: flex;
            justify-content: space-between;
            align-items: center;
            box-sizing: border-box;
        }

        .banner-title {
            font-weight: bold;
            font-size: 1.2rem;
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .doc-link {
            background-color: var(--success);
            color: white;
            text-decoration: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: 600;
            transition: background 0.2s;
            display: flex;
            align-items: center;
            gap: 8px;
        }

        .doc-link:hover {
            background-color: #059669;
        }

        /* --- FIN BANDEAU --- */

        header {
            text-align: center;
            margin-bottom: 40px;
            margin-top: 20px;
        }

        h1 { margin: 0; font-size: 2.5rem; color: var(--primary); }
        p.subtitle { color: var(--text-muted); margin-top: 5px; }

        .container {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
            gap: 25px;
            width: 90%;
            max-width: 1200px;
            margin-bottom: 50px;
        }

        .card {
            background: var(--card-bg);
            border-radius: 12px;
            padding: 25px;
            box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1);
            border: 1px solid var(--border);
            transition: transform 0.2s;
            display: flex;
            flex-direction: column;
        }

        .card:hover { transform: translateY(-3px); }

        h2 {
            margin-top: 0;
            border-bottom: 2px solid var(--bg);
            padding-bottom: 10px;
            font-size: 1.25rem;
            display: flex;
            align-items: center;
            gap: 10px;
            color: var(--text-main);
        }

        /* Badges pour la scale */
        .badge {
            font-size: 0.7em;
            background: #001eff1c;
            color: var(--primary);
            padding: 2px 6px;
            border-radius: 4px;
            margin-left: auto;
        }

        .badge-green {
            font-size: 0.7em;
            background: #00ff081b;
            color: var(--success);
            padding: 2px 6px;
            border-radius: 4px;
            margin-left: auto;
        }

        .badge-red {
            font-size: 0.7em;
            background: #ff00001a;
            color: var(--danger);
            padding: 2px 6px;
            border-radius: 4px;
            margin-left: auto;
        }

        /* Liens et Boutons */
        a.btn-link {
            display: block;
            background: var(--bg);
            padding: 12px;
            margin-bottom: 8px;
            border-radius: 6px;
            text-decoration: none;
            color: var(--text-main);
            font-weight: 500;
            transition: background 0.2s, border-left-width 0.1s;
            border-left: 4px solid var(--primary);
            font-size: 0.95rem;
        }

        a.btn-link:hover { background: #e0e7ff; border-left-width: 8px; }
        a.btn-link span { float: right; color: var(--text-muted); font-size: 0.85em; }

        /* Formulaires */
        form { display: flex; flex-direction: column; gap: 10px; margin-top: auto; }

        label { font-size: 0.9rem; font-weight: 600; color: var(--text-muted); }

        input[type="text"], input[type="file"], input[type="password"] {
            padding: 10px;
            border: 1px solid var(--border);
            border-radius: 6px;
            width: 100%;
            box-sizing: border-box;
        }

        button, input[type="submit"] {
            background-color: var(--primary);
            color: white;
            border: none;
            padding: 10px 15px;
            border-radius: 6px;
            cursor: pointer;
            font-weight: 600;
            transition: background 0.2s;
            margin-top: 5px;
        }

        button:hover, input[type="submit"]:hover { background-color: var(--primary-hover); }

        /* Styles spécifiques DELETE */
        .delete-section { margin-top: 15px; padding-top: 15px; border-top: 1px dashed var(--border); }
        .btn-danger { background-color: var(--danger); }
        .btn-danger:hover { background-color: #dc2626; }

        /* Utilitaires couleurs bordures */
        .border-red { border-left-color: var(--danger) !important; }
        .border-orange { border-left-color: var(--warning) !important; }
        .border-blue { border-left-color: var(--info) !important; }
        .border-green { border-left-color: var(--success) !important; }


    </style>
</head>
<body>

    <div class="evaluator-banner">
        <div class="banner-title">
            <span>🛡️ Webserv Evaluation</span>
        </div>
        <div>
            <a href="https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status" target="_blank" class="doc-link">
                <span>📖 Voir les Codes HTTP</span>
            </a>
        </div>
    </div>

    <header>
        <h1>🌐 Webserv </h1>
        <p class="subtitle">Serveur HTTP/1.1 compatible C++98.</p>
    </header>

    <div class="container">

        <div class="card" style="border-color: #ffd700;">
            <h2>
				<span class="icon">📝</span>
				Q&A
				<span class="badge">
					<span class="icon">ℹ️</span>
					This Q&A page is generated and managed by a .php script.
				</span>
				<span class="badge">Bonus</span>
			</h2>
            <p style="font-size: 0.9em; color: gray;">
				This quick Q&A serves as a memory test for the following features: 
				<span class="badge">[C++ server cookies]</span>
				<span>, </span>
				<span class="badge">[C++ server sessions]</span>
				<span> and </span>
				<span class="badge">[PHP sessions]</span>
			</p>

            <form action="/cgi-bin/Q&A.php" method="POST">
                <label>
					What is your favourite colour ?
                    <?php if ($_SESSION['q1'] == 'Invalid Input'): ?>
					    <span class="badge-red"> <?php echo $_SESSION['q1']; $_SESSION['q1'] = ''; ?></span>
                    <?php elseif (isset($_SESSION['q1']) && !empty($_SESSION['q1'])): ?>
					    <span class="badge-green"> <?php echo $_SESSION['q1'] ?></span>
                    <?php endif ?>
				</label>
                <input type="text" name="q1" minlength="1" maxlength="100"
                        pattern="^[a-zA-Z0-9_-]{1,100}$" title="1-100 chars (a-zA-Z0-9_-)">
				<label>
                    What is your favourite food ?
                    <?php if ($_SESSION['q2'] == 'Invalid Input'): ?>
					    <span class="badge-red"> <?php echo $_SESSION['q2']; $_SESSION['q2'] = ''; ?></span>
                    <?php elseif (isset($_SESSION['q2']) && !empty($_SESSION['q2'])): ?>
					    <span class="badge-green"> <?php echo $_SESSION['q2'] ?></span>
                    <?php endif ?>
				</label>
                <input type="text" name="q2" minlength="1" maxlength="100"
                        pattern="^[a-zA-Z0-9_-]{1,100}$" title="1-100 chars (a-zA-Z0-9_-)">
				<label>
                    What is your favourite animal ?
                    <?php if ($_SESSION['q3'] == 'Invalid Input'): ?>
					    <span class="badge-red"> <?php echo $_SESSION['q3']; $_SESSION['q3'] = ''; ?></span>
                    <?php elseif (isset($_SESSION['q3']) && !empty($_SESSION['q3'])): ?>
					    <span class="badge-green"> <?php echo $_SESSION['q3'] ?></span>
                    <?php endif ?>
				</label>
				<input type="text" name="q3" minlength="1" maxlength="100"
                        pattern="^[a-zA-Z0-9_-]{1,100}$" title="1-100 chars (a-zA-Z0-9_-)">
				<label>
                    What is your favourite season [Spring | Summer | Fall | Winter] ?
                    <?php if ($_SESSION['q4'] == 'Invalid Input'): ?>
					    <span class="badge-red"> <?php echo $_SESSION['q4']; $_SESSION['q4'] = ''; ?></span>
                    <?php elseif (isset($_SESSION['q4']) && !empty($_SESSION['q4'])): ?>
					    <span class="badge-green"> <?php echo $_SESSION['q4'] ?></span>
                    <?php endif ?>
				</label>
				<input type="text" name="q4" minlength="1" maxlength="100"
                        pattern="^[a-zA-Z0-9_-]{1,100}$" title="1-100 chars (a-zA-Z0-9_-)">
				<label>
                    What is your favourite type of music ?
                    <?php if ($_SESSION['q5'] == 'Invalid Input'): ?>
					    <span class="badge-red"> <?php echo $_SESSION['q5']; $_SESSION['q5'] = ''; ?></span>
                    <?php elseif (isset($_SESSION['q5']) && !empty($_SESSION['q5'])): ?>
					    <span class="badge-green"> <?php echo $_SESSION['q5'] ?></span>
                    <?php endif ?>
				</label>
				<input type="text" name="q5" minlength="1" maxlength="100"
                        pattern="^[a-zA-Z0-9_-]{1,100}$" title="1-100 chars (a-zA-Z0-9_-)">
				<label>
                    What is your dream vacation destination ?
                    <?php if ($_SESSION['q6'] == 'Invalid Input'): ?>
					    <span class="badge-red"> <?php echo $_SESSION['q6']; $_SESSION['q6'] = ''; ?></span>
                    <?php elseif (isset($_SESSION['q6']) && !empty($_SESSION['q6'])): ?>
					    <span class="badge-green"> <?php echo $_SESSION['q6'] ?></span>
                    <?php endif ?>
                </label>
				<input type="text" name="q6" minlength="1" maxlength="100"
                        pattern="^[a-zA-Z0-9_-]{1,100}$" title="1-100 chars (a-zA-Z0-9_-)">
				<label>
                    What is your favourite hobby or free time activity ? 
                    <?php if ($_SESSION['q7'] == 'Invalid Input'): ?>
					    <span class="badge-red"> <?php echo $_SESSION['q7']; $_SESSION['q7'] = ''; ?></span>
                    <?php elseif (isset($_SESSION['q7']) && !empty($_SESSION['q7'])): ?>
					    <span class="badge-green"> <?php echo $_SESSION['q7'] ?></span>
                    <?php endif ?>
				</label>
				<input type="text" name="q7" minlength="1" maxlength="100"
                        pattern="^[a-zA-Z0-9_-]{1,100}$" title="1-100 chars (a-zA-Z0-9_-)">
				<label>
                    What is your favourite Movie / Series / TV Show ?
                    <?php if ($_SESSION['q8'] == 'Invalid Input'): ?>
					    <span class="badge-red"> <?php echo $_SESSION['q8']; $_SESSION['q8'] = ''; ?></span>
                    <?php elseif (isset($_SESSION['q8']) && !empty($_SESSION['q8'])): ?>
					    <span class="badge-green"> <?php echo $_SESSION['q8'] ?></span>
                    <?php endif ?>
				</label>
				<input type="text" name="q8" minlength="1" maxlength="100"
                        pattern="^[a-zA-Z0-9_-]{1,100}$" title="1-100 chars (a-zA-Z0-9_-)">
                <input type="submit" value="Send Answers" style="background-color: #f59e0b;">
            </form>
        </div>
    </div>
    <a href="/" class="btn-link">
        <span>🏠 Return to Home Page</span>
    </a>
</body>
</html>

<?php // Session END
session_write_close();
?>