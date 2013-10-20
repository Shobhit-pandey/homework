Choosing a Project
==================

Assignment 1 for CS419 - Special Topics in Open Source Development

Project: ReadTheDocs
URL: http://readthedocs.org, http://github.com/rtfd

Project Membership:
  Contributers: 3 - 5 Active, 62 Total
  Leaders: Eric Holscher, Chris McDonald, Charles Leifer 
    Head: Eric Holscher
    VP: Chris McDonald

Description of infrastructure/architecture + links
--------------------------------------------------
Infrastructure: http://docs.readthedocs.org/en/latest/rtfd.html
Architecture: http://docs.readthedocs.org/en/latest/architecture.html 

What the project is about
-------------------------
ReadTheDocs (RTD) hosts documentation for open source communities. It
generates and hosts documentation, while integrating with a wide array
of source configuration management tools.

RTD is built on top of existing technology such as Django, Sphinx-doc, and
Celery. The project has three main goals: encourage people to write
documentation, be a central platform for finding documentation, and
improving the quality of documentation.

It is licensed under the BSD License. This is mainly because Django is
licensed under BSD as well.

.. _why-rtd-matters: http://ericholscher.com/blog/2012/jan/22/why-read-docs-matters/

Brief History
-------------
ReadTheDocs was started in 2010 by Charles Leifer, Bobby Grace, and Eric
Holscher at the Django Dash.

They saw that documentation hosting was pretty bad in the python world.
The standard involved uploading a collection of html pages to
package.python.org. Yet some people were transitioning to using github
pages, and other had just given up and succumbed to using a collection
of cron jobs.

The creaters knew that a better alternative could be made. Their
choice of technology was mainly do to familiarity and .. Since most
of their expertise was in Python and Django, that is what they used.
This included Sphinx-doc which was already being used in the Python
project for documentation. Useing these tools they were able to create
a working website in two days. From there the project grew to having
over 80,000 page views a day. Documentation was being hosted.

A more in depth explination of the project and it's goals can be found by
watching Eric Holscher's talk at DjangoCon 2011 here:
http://lanyrd.com/2011/djangocon-us/sgfwh/


Rational for selection & what you want to contribute
----------------------------------------------------
I currently use ReadTheDocs (RTD) to host documentation for Ganeti Web
Manager (GWM), an open source project from the Oregon State University
Open Source Lab. I have really enjoyed using the site and it makes
writing and hosting documentation much easier.

I have two main items I want to contribute. The first is support for
cloning a project. The second is created a better error page.
