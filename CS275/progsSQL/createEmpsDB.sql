create table emps (
  name     char(10)  not null,
  jobClass char(16)  not null,
  salary   integer   not null,
  primary  key (name)
);

insert into emps
  values('Chang', 'Programmer', 30000);
insert into emps
  values('Cohen', 'Technician', 30000);
insert into emps
  values('Honda', 'Secretary ', 25000);
insert into emps
  values('Smith', 'Stenographer', 25000);
