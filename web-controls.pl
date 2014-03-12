#! /usr/bin/env perl

use Mojolicious::Lite;
use Mojolicious::Static;

get '/' => sub {
	shift->render('index');
};

app->static->paths->[0] = app->home;
app->start;
