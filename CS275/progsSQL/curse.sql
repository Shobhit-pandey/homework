DECLARE CURSOR X IS
  SELECT name, jobClass
  FROM   emps
  FOR UPDATE OF salary;

  name     char(10);
  jclass   char(16);
  rate     number;

BEGIN
  OPEN X;
  FETCH X INTO name, jclass;
  WHILE X%FOUND LOOP
    IF jclass = 'Programmer' THEN
      rate := 1.20;
    else 
      rate := 1.10;
    END IF;

    dbms_output.put_line('Rate of salary increase of ' || name || ' is '
                        || to_char(rate));

    UPDATE emps
      SET salary = salary * rate
      WHERE CURRENT OF X;
    FETCH X INTO name, jclass;
  END LOOP;
END;

