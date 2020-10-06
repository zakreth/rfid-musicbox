#ifndef WebContent_h
#define WebContent_h

const char HTML[] PROGMEM = R"rawliteral(


<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <link href="https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-Vkoo8x4CGsO3+Hhxv8T/Q5PaXtkKtu6ug5TOeNV6gBiFeWPGFN9MuhOf23Q9Ifjh" crossorigin="anonymous">
    <title></title>
  </head>
  <body>
    <div class="container">
      <div id="alertBox" class="alert alert-primary d-none" role="alert">
        HOLDING TEXT
      </div>
      <div class="row">
        <div class="col"><h1>&#9834; Music<small>BOX</small></h1></div>
      </div>
      <form>
        <div class="row">
          <div class="col-md-4">
            <label for="type">Option</label>
            <select class="custom-select d-block w-100" id="type" required>
              <option value="">Choose...</option>
              <option value="PLAY">Play Item</option>
              <option value="LOCATION">Set Audio Destination</option>
              <option value="STOP">Stop/Pause Playback</option>
              <option value="LOCK">Lock System</option>
            </select>
          </div>
          <div class="col-md-8">
            <div id="sectionPlayBack" class="collapse">
              <label for="playBackItem">Playback URL</label>
              <input type="text" class="form-control" id="playBackItem" placeholder="URL" />
              <small><a href="https://open.spotify.com" target="_blank">Open Spotify Web Player</a></small>
              <input type="text" class="form-control d-none" id="playBackURI" placeholder="" />
            </div>
            <div id="sectionLocation" class="collapse">
              <label for="locationItem">Location <small>(<a href="javascript:void(0);" id="refreshLocations">refresh</a>)</small></label>
              <select type="text" class="form-control" id="locationDropdown" placeholder="URL"></select>
            </div>
          </div>
        </div>
        <div class="row">&nbsp;</div>
        <div class="row">
          <div class="col-md-12">
            <div class="btn-group">
              <button type="submit" id="writeCardButton" class="btn btn-primary">Write to Card</button>
              <button id="loadingButton" class="btn btn-primary d-none" type="button" disabled>
                <span class="spinner-border spinner-border-sm" role="status" aria-hidden="true"></span>
                <span id="loadingButtonText">Loading...</div>
              </button>
              <button id="cancelWriteButton" class="btn btn-danger d-none" type="button">
                <span id="cancelWriteButtonText">Cancel</div>
              </button>
            </div>
          </div>
        </div>
      </form>
    </div>
    <script src="https://code.jquery.com/jquery-3.5.1.min.js" integrity="sha256-9/aliU8dGd2tb6OSsuzixeV4y/faTqgFtohetphbbj0=" crossorigin="anonymous"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
    <script src="https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/js/bootstrap.min.js" integrity="sha384-wfSDF2E50Y2D1uUdj0O3uMBJnjuUD4Ih7YwaYd1iqfktj0Uod8GCExl3Og8ifwB6" crossorigin="anonymous"></script><script>
      
      var g_countdown;
      
      function getLocations() {
        let dropdown = $('#locationDropdown');

        dropdown.empty();
        dropdown.append('<option selected="true" disabled>Choose Location</option>');
        dropdown.prop('selectedIndex', 0);

        const url = '/locations';

        // Populate dropdown with list of provinces
        $.getJSON(url, function (data) {
          $.each(data, function (key, val) {
            dropdown.append($('<option value="' + key + '">' + val + '</option>'));
          })
        });
      }

      function cancelWriteRequest(submit_request) {
        $('#writeCardButton').removeClass('d-none');
        $('#loadingButton').addClass('d-none');
        $('#cancelWriteButton').addClass('d-none');
        clearInterval(g_countdown);

        if (submit_request) {
          // Submit a cancel request to the server
          const url = '/writecancel';

          $.get(url, function() {
            showAlert(3, 'Write request cancelled successfully.');
          })
          .fail(function() {
            showAlert(3, 'Error cancelling write request. Please try again.');
          });
        }
      }

      function showAlert(type, text) {
        var element = $('#alertBox');

        element.html(text);

        if (type == 0) {
          element.addClass('d-none'); // Hide
        } else {
          switch (type) {
            case 1 : element.attr('class', 'alert alert-primary'); break;
            case 2 : element.attr('class', 'alert alert-success'); break;
            case 3 : element.attr('class', 'alert alert-danger'); break;
          };

          element.removeClass('d-none'); // Show
        }
      }

      function updateTitle() {
        var url = '/name';

        // Submit the request
        $.get(url, function(data) {
          // Successfully submitted
          $(document).prop('title', data);
        })
        .fail(function() {
          // Fail quietly
        });
      }

      $('#refreshLocations').on('click', getLocations);
      $('#cancelWriteButton').on('click', function() {
        showAlert(2, 'Write request has been cancelled');
        cancelWriteRequest(true);
      });

      $('#playBackItem').on('change', function() {
        if ($(this).val().substring(0, 25) == 'https://open.spotify.com/') {
          var splitArray = $(this).val().substring(25).split('#')[0].split('?')[0].split('/').reverse();

          if (splitArray.length > 1)
          {
            $('#playBackURI').val('x-sonos-spotify:spotify' + encodeURIComponent(':' + splitArray[1] + ':' + splitArray[0]));
          }
        }
      });

      $('#writeCardButton').on('click', function() {
        var submit = false;
        var url = '/write?type=' + $('#type').val();

        if (($('#type').val() == "STOP") || ($('#type').val() == "LOCK")) {
          submit = true;
        } else if ($('#type').val() == "PLAY") {
          if ($('#playBackURI').val() == '') {
            showAlert(3, 'Please specify a URL to play before submitting');
          } else {
            url += '&url=' + $('#playBackURI').val();
            submit = true;
          }
        } else if ($('#type').val() == "LOCATION") {
          if (!$('#locationDropdown').val()) {
            showAlert(3, 'Please specify a Location before submitting');
          } else {
            url += '&location=' + $('#locationDropdown').val();
            submit = true;
          }
        } else {
          showAlert(3, 'Please specify an option first');
        }

        if (submit) {
          showAlert(1, 'Please hold RFID card to reader to update changes...');
          $('#writeCardButton').addClass('d-none');
          $('#loadingButton').removeClass('d-none');
          $('#cancelWriteButton').removeClass('d-none');

          // Submit the request
          $.get(url, function() {
            // Successfully submitted
          })
          .fail(function() {
            showAlert(3, 'Something went wrong while submitting your request. Please try again.');
          });

          // Update the UI
          var counter = 10;

          g_countdown = setInterval(function() {
            $('#loadingButtonText').html('Loading... (' + counter + ')');

            counter -= 1;

            if (counter < 0) {
              cancelWriteRequest(false);
              showAlert(0, '');
            }
          }, 1000);
        }

        return false;
      });

      $('#type').change( function() {
        console.log($( this ).val());

        $('#sectionLocation').hide();
        $('#sectionPlayBack').hide();

        switch ($( this ).val()) {
          case "PLAY" :
            $('#sectionPlayBack').show();
            break;
          case "LOCATION" :
            $('#sectionLocation').show();
            break;
        }
      });



      getLocations();
      updateTitle();
    </script>
  </body>
</html>


)rawliteral";

#endif