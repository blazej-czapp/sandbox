var NO_OF_CHARACTERS_PER_COLUMN = 204;
var COLUMN_WIDTH = 12;
var LEFT_COLUMN = "leftCol";
var RIGHT_COLUMN = "rightCol";
var WORD_PROBABILITY = 0.05; // how often should words be written, as opposed to garbage
var PRINT_TIMEOUT = 1; // time between character prints
var INITIAL_ATTEMPTS = 5;
var charactersWritten = {};

var garbageChars = '!"$%^&*()_+-=[]{};\'#:@~,./<>?`\\|'
var validWords = ["breaking", "creating", "guardian", "document", "agreeing", "greenery", "dynamite", "facility", "tripping", "stemming", "loyalist", "rustling", "chambers", "breakers", "brawling", "thinking", "cleaning"];
var usedWords;

var printedWord; // currently printed word
var wordPrintedSoFar = 0; // how much of the currently written word has been written already
var timeout_var;
var correctAnswer;
var output = document.getElementById("output");

var currentHyper = 0; // id of the currently written hyperlink (for word)
var lineNumber = 8235; // some arbitrary number, so the hex looks nice

var TIME_LIMIT = 300;
var timeRemaining;
var intervalTimer;
var timerElement = document.getElementById('timer');

var score = 0;
var scoreElement = document.getElementById('score');
var finalScoreElement = document.getElementById('finalScore');

var attemptsLeft = INITIAL_ATTEMPTS;
var attemptsElement = document.getElementById('attempts');


var game = document.getElementById('game');
var usernameForm = document.getElementById('usernameForm');
var scoreboard = document.getElementById('scoreboard');

var usernameField = document.getElementById('usernameField');
var scoreboardData = document.getElementById('scoreboardData');

// maxLengthOfWord - indicate how much space is left in the current column - can't spill hyperlink to another column, so don't print if there isn't enough
function getNextCharacter(maxLengthOfWord)
{
  if (printedWord === undefined && Math.random() < WORD_PROBABILITY && validWords.length > 0)
  {
    candidate = getRandomWord();
    
    if (candidate !== undefined && candidate.length <= maxLengthOfWord)
    {
      printedWord = candidate;
      usedWords[usedWords.length] = printedWord;
      wordPrintedSoFar = 0;
    } // if word won't fit, just fall through and generate a random characted
  }

  if (printedWord !== undefined)
  {
    if (wordPrintedSoFar < printedWord.length)
    {
      return printedWord[wordPrintedSoFar++];
    }
    else
    {
      printedWord = undefined;
      return garbageChars[Math.floor(Math.random() * garbageChars.length)];
    }
  }
  else
  {
    return garbageChars[Math.floor(Math.random() * garbageChars.length)];
  }
}

function printCharacter(whichColumn)
{
  if (charactersWritten[whichColumn] % COLUMN_WIDTH == 0)
  {
    document.getElementById(whichColumn + "Labels").innerHTML += "0x" + lineNumber.toString(16) + "<br>";
    lineNumber += 8;
    if (charactersWritten[whichColumn] > 0) // don't print <br> immediately in the first line
    {
      if (printedWord === undefined)
      {
        document.getElementById(whichColumn).innerHTML += "<br>";
      }
      else
      {
        document.getElementById("hyper" + currentHyper).innerHTML += "<br>";
      }
    }
  }

  var nextChar = getNextCharacter(NO_OF_CHARACTERS_PER_COLUMN - charactersWritten[whichColumn]);

  if (garbageChars.indexOf(nextChar) == -1) // i.e. it's a word char
  {
    if (wordPrintedSoFar == 1) // just starting, so create the hyperlink tag (also used for highlighting)
    {
      currentHyper++;
      var cell = document.getElementById(whichColumn);
      a = document.createElement('a');
      a.href = 'javascript:void(0);';
      a.setAttribute('onclick', 'wordClicked("' + printedWord + '");');
      a.setAttribute('id', 'hyper' + currentHyper);
      cell.appendChild(a);
    }
    
    document.getElementById("hyper" + currentHyper).innerHTML += nextChar;
  }
  else
  {
    // create a small anchor for each character, so they can be highlighted individually
    document.getElementById(whichColumn).innerHTML += '<a class="garbage" href="#">' + nextChar + '</a>';
  }

  charactersWritten[whichColumn]++;
}

function wordClicked(word)
{
  if (systemLocked || timeRemaining <= 0)
  {
    return;
  }

  var i;
  var numCorrect = 0;
  for (i = 0; i < word.length; i++)
  {
    if (word[i] === correctAnswer[i])
    {
      numCorrect++;
    }
  }

  if (numCorrect === correctAnswer.length)
  {
    score++;
    updateScore();
    output.innerHTML += "Correct password: " + word + ". ACCESS GRANTED<br>";
    buildPuzzle();
  }
  else
  {
    attemptsLeft--;
    updateAttempts();
    if (attemptsLeft <= 0)
    {
      output.innerHTML += "ACCESS DENIED, SYSTEM LOCKED<br>";
      endGame();
    }
    else
    {
      if (attemptsLeft === 1)
      {
        var warningMessage = document.createElement('div');
        warningMessage.setAttribute("class", "warning");
        warningMessage.setAttribute("id", "warningMessage");
        warningMessage.innerHTML = "Incorrect password: " + word + ", " + numCorrect + "/" + correctAnswer.length + " correct. LOCKDOWN IMMINENT<br>";
        output.appendChild(warningMessage);
      }    
      else
      {
        output.innerHTML += "Incorrect password: " + word + ", " + numCorrect + "/" + correctAnswer.length + " correct. Attempts left: " + attemptsLeft + "<br>";
      }
    }
  }
}

// gets a random unused word
function getRandomWord()
{
  var options = [];
  for (word in validWords)
  {
    var found = false;
    for (used in usedWords)
    {
      if (validWords[word] === usedWords[used])
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      options[options.length] = validWords[word];
    }
  }
  return options[Math.floor(Math.random() * options.length)];
}

function dispatch()
{
  if (charactersWritten[LEFT_COLUMN] < NO_OF_CHARACTERS_PER_COLUMN)
  {
    printCharacter(LEFT_COLUMN);
    timeout_var = setTimeout(dispatch, PRINT_TIMEOUT);
  }
  else if (charactersWritten[RIGHT_COLUMN] < NO_OF_CHARACTERS_PER_COLUMN)
  {
    printCharacter(RIGHT_COLUMN);
    timeout_var = setTimeout(dispatch, PRINT_TIMEOUT);
  }
  else
  {
    correctAnswer = usedWords[Math.floor(Math.random() * usedWords.length)];
    systemLocked = false;
    output.innerHTML = "";
  }
}

function buildPuzzle()
{
  usedWords = [];
  document.getElementById(LEFT_COLUMN).innerHTML = "";
  document.getElementById(RIGHT_COLUMN).innerHTML = "";
  document.getElementById(LEFT_COLUMN + "Labels").innerHTML = "";
  document.getElementById(RIGHT_COLUMN + "Labels").innerHTML = "";
  charactersWritten[LEFT_COLUMN] = 0;
  charactersWritten[RIGHT_COLUMN] = 0;
  systemLocked = true;
  attemptsLeft = INITIAL_ATTEMPTS;
  updateAttempts();
  dispatch();
}

function displayTimer()
{
  var minuteString = "" + (timeRemaining % 60);
  if (minuteString.length < 2)
  {
    minuteString = '0' + minuteString;
  }
  timerElement.textContent = Math.floor(timeRemaining / 60) + ':' + minuteString;
}

function updateScore()
{
  scoreElement.textContent = score;
}

function updateAttempts()
{
  attemptsElement.textContent = attemptsLeft;
}

function startTimer()
{
  score = 0;
  timeRemaining = TIME_LIMIT;
  intervalTimer = setInterval(timerStep, 1000);
  displayTimer();
  updateScore();
}

function timerStep()
{
  timeRemaining--;
  displayTimer();
  if (timeRemaining === 0)
  {
    endGame();
    output.innerHTML += 'SESSION TIMER EXPIRED, SYSTEM LOCKED.<br>';
  }
}

function endGame()
{
  clearInterval(intervalTimer);
  output.innerHTML = '';
  systemLocked = true;

  finalScoreElement.textContent = scoreElement.textContent;

  game.style.display = 'none';
  usernameForm.style.display = 'block';
}

function submitUsername()
{
  addScore(usernameField.value, finalScoreElement.textContent);
  usernameField.value = '';
  showScoreboard();
  // prevent form submission (i.e. page reload)
  return false;
}

function addScore(username, value)
{
  if (typeof localStorage.scores === 'undefined')
  {
    localStorage.setItem('scores', JSON.stringify(new Array()));
  }
  
  var newScores = JSON.parse(localStorage.getItem('scores'))
  newScores[newScores.length] = { 'username' : username, 'score' : value };
  localStorage.setItem('scores', JSON.stringify(newScores));
}

function showScoreboard()
{
  // remove old elements
  var oldScores = [];
  for (var i = 0; i < scoreboardData.children.length; i++)
  {
    if (scoreboardData.children[i].getElementsByTagName('TH').length == 0)
    {
      oldScores[oldScores.length] = scoreboardData.children[i];
    }
  }
  for (var i = 0; i < oldScores.length; i++)
  {
    scoreboardData.removeChild(oldScores[i]);
  }

  var data = JSON.parse(localStorage.getItem('scores'))
  data.sort(function(a,b)
  {
    if (parseInt(b.score) - parseInt(a.score) != 0)
    {
      return parseInt(b.score) - parseInt(a.score);
    }
    return a.username.localeCompare(b.username);
  });

  for (var i = 0; i < data.length && i < 10; i++)
  {
    var positionElement = document.createElement('TD');
    positionElement.className = 'positionValue'
    positionElement.textContent = i + 1;
    var nameElement = document.createElement('TD');
    nameElement.textContent = data[i].username;
    var scoreElement = document.createElement('TD');
    scoreElement.className = 'scoreValue'
    scoreElement.textContent = data[i].score;
    var row = document.createElement('TR');
    row.appendChild(positionElement);
    row.appendChild(nameElement);
    row.appendChild(scoreElement);
    scoreboardData.appendChild(row);
  }

  game.style.display = 'none';
  usernameForm.style.display = 'none';
  scoreboard.style.display = 'block';
}

function restartGame()
{
  usernameForm.style.display = 'none';
  scoreboard.style.display = 'none';
  game.style.display = 'block';
  buildPuzzle();
  startTimer();
}

function clearScores()
{
  localStorage.clear();
}

buildPuzzle();
startTimer();
