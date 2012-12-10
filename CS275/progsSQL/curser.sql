DECLARE salaryUpdate CURSOR FOR
    SELECT name, jobClass
    FROM   emps
    FOR UPDATE OF salary;
DECLARE  @name     char(10);
DECLARE  @jclass   char(16);
DECLARE  @rate     float;

OPEN salaryUpdate;

FETCH salaryUpdate INTO @name, @jclass;

WHILE (@@FETCH_STATUS = 0) 
BEGIN
  IF @jclass = 'Programmer'
    set @rate = 1.20;
  ELSE 
    set @rate = 1.10;
  PRINT 'Rate of salary increase of ' + @name +
    ' is ' + cast(@rate as varchar(8));
  UPDATE emps
    SET salary = salary * @rate
    WHERE CURRENT OF salaryUpdate;
  FETCH salaryUpdate INTO @name, @jclass;
END;
