drop table employee;

create table employee (
  employee_id   int primary key auto_increment,
  name          varchar(10)  not null,
  job_class     varchar(16)  not null,
  salary        integer   not null
);

insert into employee(name, job_class, salary)
values('Chang', 'Programmer', 30000);

insert into  employee(name, job_class, salary)
values('Cohen', 'Technician', 30000);

insert into  employee(name, job_class, salary)
values('Honda', 'Secretary', 25000);

insert into  employee(name, job_class, salary)
values('Smith', 'Stenographer', 25000);

