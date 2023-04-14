t <html><head><title>Control de Vibración</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var VibraUpdate = new periodicObj("Vibracion.cgx", 1000);
t function updateValueVibra() {
t  VibraVal = document.getElementById("vibra").value;
t }
t function updateVibra() {
t  Val_temp = document.getElementById("vibra").value;
t }
t function periodicupdateVibra() {
t  if(document.getElementById("adChkBox").checked == true) {
t   updateMultiple(VibraUpdate,updateValueVibra);
t   val_elVibra = setTimeout(periodicupdateVibra, VibraUpdate.period);
t  }
t  else
t   clearTimeout(val_elVibra);
t }
t  </script> </head>
i pg_header.inc
t <h2 align=center><br>Modulo de Vibracion</h2>
t <p><font size="2">Esta página muestra el valor de la vibracion.
t <form action="Vibracion.cgi" method="post" name="vibracion">
t <input type="hidden" value="temperatura" name="pg">
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#d0f37b>
t  <th width=30%>Vibracion</th>
t <tr>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px text-align:"center""
c t size="60" id="vibra" value="%5.3f "></td>
t </font></table>
t <p align=center>
t <input type=button value="Actualizar Vibracion" onclick="updateMultiple(updateVibra,updateValueVibra)">
t <input type="checkbox" id="adChkBox" onclick="periodicupdateVibra()"> Actualización periódica
t </p></form>
i pg_footer.inc
. End of script must be closed with period.