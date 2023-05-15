t <html><head><title>Control de Luminosidad</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var LuxUpdate = new periodicObj("Luminosidad.cgx", 1000);
t function updateValueLux() {
t  LuxVal = document.getElementById("valor_lum").value;
t }
t function periodicUpdateLux() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(LuxUpdate,updateValueLux);
t   val_elLux = setTimeout(periodicUpdateLux, LuxUpdate.period);
t  }
t  else
t   clearTimeout(val_elLux);
t }
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Modulo de Luminosidad</h2>
t <p><font size="2">Esta página muestra el valor de la <b> luminosidad <b>.
t <form action="Luminosidad.cgi" method="post" name="luminosidad">
t <input type="hidden" value="luminosidad" name="pg">
t <br>
t </form>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#FFF52D>
t  <th width=30%>Luminosidad (%)</th>
t <tr>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px text-align:"center""
c l 1 size="60" id="valor_lum" value="%s "></td>
t </font></table>
t <p align=center>
t <input type="checkbox" id="adChkBox" onclick="periodicUpdateLux()"> Actualización periódica
t </p></form>
i pg_footer.inc
. End of script must be closed with period.