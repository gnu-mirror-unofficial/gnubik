DOMAIN=$(PACKAGE)
MSGID_BUGS_ADDRESS=$(PACKAGE_BUGREPORT)

XGETTEXT=xgettext
MSGMERGE=msgmerge
MSGFMT=msgfmt


POFILES=\
	po/ar.po \
	po/bg.po \
	po/ca.po \
	po/da.po \
	po/de.po \
	po/el.po \
	po/en_US.po \
	po/eo.po \
	po/es.po \
	po/eu.po \
	po/fi.po \
	po/fr.po \
	po/fur.po \
	po/gl.po \
	po/he.po \
	po/hr.po \
	po/it.po \
	po/ms.po \
	po/nb.po \
	po/nl.po \
	po/pl.po \
	po/pt_BR.po \
	po/pt.po \
	po/tr.po \
	po/uk.po \
	po/ro.po \
	po/ru.po \
	po/sl.po \
	po/sr.po \
	po/sv.po \
	po/vi.po \
	po/zh_CN.po 


POTFILE=po/$(DOMAIN).pot

COPYRIGHT_HOLDER=John Darrington

XGETTEXT_OPTIONS = \
	--copyright-holder="$(COPYRIGHT_HOLDER)" \
	--package-name=$(PACKAGE) \
	--package-version=$(VERSION) \
	--msgid-bugs-address=$(MSGID_BUGS_ADDRESS) \
	--from-code=UTF-8 \
	--add-comments='TRANSLATORS:'

$(POTFILE): $(DIST_SOURCES) $(dist_script_DATA) $(desktop_DATA) $(top_srcdir)/icons/gen-dot-desktop.scm
	@$(MKDIR_P) po
	$(XGETTEXT) --directory=$(top_srcdir) $(XGETTEXT_OPTIONS) $(DIST_SOURCES) --language=C --keyword=_ --keyword=N_ -o $@
	$(XGETTEXT) --directory=$(top_srcdir) $(XGETTEXT_OPTIONS) $(dist_script_DATA) $(desktop_DATA) $(top_srcdir)/icons/gen-dot-desktop.scm --language=scheme --keyword=_ --keyword=N_ -j -o $@
	$(SED) -e '/POT-Creation-Date: /d' $@ > $@,tmp
	mv $@,tmp $@


$(POFILES): $(POTFILE)
	$(MSGMERGE) $(top_srcdir)/$@ $? -o $@


SUFFIXES += .po .gmo
.po.gmo:
	@$(MKDIR_P) `dirname $@`
	$(MSGFMT) $< -o $@


GMOFILES = $(POFILES:.po=.gmo)

ALL_LOCAL += $(GMOFILES)

install-data-hook: $(GMOFILES)
	for f in $(GMOFILES); do \
	  lang=`echo $$f | $(SED) -e 's%po/\(.*\)\.gmo%\1%' ` ; \
	  $(MKDIR_P) $(DESTDIR)$(prefix)/share/locale/$$lang/LC_MESSAGES; \
	  $(INSTALL_DATA) $$f $(DESTDIR)$(prefix)/share/locale/$$lang/LC_MESSAGES/$(DOMAIN).mo ; \
	done

uninstall-hook:
	for f in $(GMOFILES); do \
	  lang=`echo $$f | $(SED) -e 's%po/\(.*\)\.gmo%\1%' ` ; \
	  rm -f $(DESTDIR)$(prefix)/share/locale/$$lang/LC_MESSAGES/$(DOMAIN).mo ; \
	done


EXTRA_DIST += $(POFILES) $(POTFILE)

CLEANFILES += $(GMOFILES) $(POTFILE)

# Clean $(POFILES) from build directory but not if that's the same as
# the source directory.
.PHONY: po_CLEAN
po_CLEAN:
	@if test "$(srcdir)" != .; then \
		echo rm -f $(POFILES); \
		rm -f $(POFILES); \
	fi

CLEAN_LOCAL += po_CLEAN
