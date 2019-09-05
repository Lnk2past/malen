from bokeh.plotting import figure, output_file, save


def make_new_figure(title, plot_width=640, plot_height=640):
    """Create a new figure
    
    Arguments:
        title (str): name of the figure
        plot_width (int): width in pixels
        plot_height (int): height in pixels

    Returns:
        bokeh.plotting.figure.Figure object to be graphed on
    """
    fig = figure(**locals())
    return fig


def line(fig, x, y, **kwargs):
    """Create a line object
    
    Arguments:
        x (list): x data
        y (list): y data
        kwargs: Line properties and Fill properties
    """
    fig.line(x, y, **kwargs)


def scatter(fig, x, y, **kwargs):
    """Create a scatter object
    
    Arguments:
        x (list): x data
        y (list): y data
        kwargs: Line properties and Fill properties
    """
    fig.scatter(x, y, **kwargs)


def generate_html(fig, filename, **kwargs):
    """Generate and save the HTML of the figure
    
    Arguments:
        filename (str): name of the file to save the figure to
        kwargs: additional output properties
    """
    output_file(filename, **kwargs)
    save(fig)
