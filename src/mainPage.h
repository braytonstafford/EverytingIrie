const char *MAIN_page = R"=====(
<!DOCTYPE html>
<html>

<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width,user-scalable=no,initial-scale=1,maximum-scale=1,minimum-scale=1">
  <script src="https://cdnjs.cloudflare.com/ajax/libs/babel-standalone/6.26.0/babel.min.js"></script>
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/antd-mobile/2.2.8/antd-mobile.min.css">
</head>

<body>
  <div id="root"></div>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/lodash.js/4.17.11/lodash.min.js"></script>
  <script crossorigin src="https://cdnjs.cloudflare.com/ajax/libs/react/16.7.0/umd/react.production.min.js"></script>
  <script crossorigin src="https://cdnjs.cloudflare.com/ajax/libs/react-dom/16.7.0/umd/react-dom.production.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/antd-mobile/2.2.8/antd-mobile.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/fetch/3.0.0/fetch.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/fetch/3.0.0/fetch.min.js.map"></script>
  <script type="text/jsx">
    console.log(window['antd-mobile']);
    const LocaleProvider = window['antd-mobile'].LocaleProvider;
    const enUS = window['antd-mobile'].LocaleProvider.en_US;
    const Button = window['antd-mobile'].Button;
    const SearchBar = window['antd-mobile'].SearchBar;
    const Picker = window['antd-mobile'].Picker;
    const List = window['antd-mobile'].List;
    const Grid = window['antd-mobile'].Grid;
    const NoticeBar = window['antd-mobile'].NoticeBar;
    const CheckboxItem = window['antd-mobile'].Checkbox.CheckboxItem;
    const BASE_URL = window.location.hostname;
    
    class App extends React.PureComponent {
      blinkStart = () => {
        fetch(`http://${BASE_URL}/startBlink`).then(res => res.json()).then(json => console.log('Blinking Started!')).catch(err => console.log('Error while attempting start blinking'))
      };
      blinkStop = () => {
        fetch(`http://${BASE_URL}/stopBlink`).then(res => res.json()).then(json => console.log('Blinking Stopped!')).catch(err => console.log('Error while attempting stop blinking'))
      };
      render() {
        return <LocaleProvider locale={enUS}>
          <div>
            <SearchBar placeholder="search here..." />
            <Button onClick={() => this.blinkStart()}>Start Blinking</Button>
            <Button onClick={() => this.blinkStop()}>Stop Blinking</Button>
          </div>
        </LocaleProvider>;
      };
    };
    ReactDOM.render(<App />, document.getElementById('root'));
  </script>
</body>

</html>
)=====";