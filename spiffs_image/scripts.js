// var configuration = {};

$(document).ready(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200)
      {
          configuration = JSON.parse(this.responseText);
          console.log(configuration);
      }
  }
  xhttp.open("GET", "get_time_slot_config", true);
  xhttp.send();
});

function handleOptionSelectedDuration(e){			
  const id = e.target.id;
  const newValue = e.target.textContent + ' ';

  //trigger custom event
  document.querySelector("#prt1_duration").value = newValue;

  var number = 0;
  for (item of configuration.ports) {
      if (item.number == "1") {
          configuration.ports[number].duration = Number(newValue) * 60;   // minutes converted to seconds
      }
      number++;
  }
  console.log(configuration);
}

function handleOptionSelectedInterval(e){			
  const id = e.target.id;
  const newValue = e.target.textContent + ' ';

  //trigger custom event
  document.querySelector("#prt1_interval").value = newValue;

  var number = 0;
  for (item of configuration.ports) {
    if (item.number == "1") {
      if (id != "option16" || id != "option17") {
        configuration.ports[number].interval = Number(newValue) * 60 * 60;  // Hours converted to seconds
        break;
      }
      else {
        configuration.ports[number].interval = Number(newValue) * 604800;  // weeks converted to seconds
        break;
      }
    }
    number++;
  }
  console.log(configuration);
}

function save_configuration(){
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "set_time_slot_config", true);
  xhttp.setRequestHeader('Accept', 'text/json');
  xhttp.send(JSON.stringify(configuration));
}

const dropdownOptionsDuration = document.querySelectorAll('.duration_button .option');
const dropdownOptionsInterval = document.querySelectorAll('.interval_button .option')

dropdownOptionsDuration.forEach(option => option.addEventListener('click',handleOptionSelectedDuration));
dropdownOptionsInterval.forEach(option => option.addEventListener('click',handleOptionSelectedInterval));
