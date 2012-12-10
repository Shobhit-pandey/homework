create or replace procedure formTest(cityName in s.city%TYPE) is
  CURSOR C1 IS
    select sname
    from s
    where city = cityName;
BEGIN
  htp.print('<H2>Suppliers in '|| cityName || ':</H2>');
  htp.print('<OL>');
  FOR x IN C1 LOOP
    htp.print('<LI>'|| x.sname);
  END LOOP;
  htp.print('</OL>');
END;