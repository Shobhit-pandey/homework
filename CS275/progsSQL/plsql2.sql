DECLARE 
  CURSOR C1(supName s.sname%TYPE) IS
    select pname
    from s, sp, p
    where sname = supName
      and s.sno = sp.sno and sp.pno = p.pno;
BEGIN
  FOR x IN C1('Smith') LOOP
    DBMS_OUTPUT.PUT_LINE(X.pname);
  END LOOP;
END;
/
