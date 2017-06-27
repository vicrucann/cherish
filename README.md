# Cherish manual site

## Local build instructions

After cloning the repository, change the branch by using: 

```
git checkout gh-pages
```

Install bundler and run it to install Jekyll and all the dependencies:

```
gem install bundler
bundle install
```

Now run Jekyll by using `bundle exec`:

```
bundle exec jekyll build
bundle exec jekyll serve
```

In case if running on a cloud service the latter command might look like: 

```
bundle exec jekyll serve --host $IP --port $PORT --baseurl ''
```

Or locally on your machine:

```
bundle exec jekyll serve --baseurl ''
```
