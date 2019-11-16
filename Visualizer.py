import itertools
import bokeh
from bokeh.io import curdoc
from bokeh.layouts import row, column
from bokeh.models import HoverTool, CustomJS, Slider, ColorBar
from bokeh.models.mappers import *
from bokeh.models.tickers import *
from bokeh.plotting import figure, output_file, save, ColumnDataSource
from bokeh.palettes import viridis as palette

colors = itertools.cycle(palette(16))

def make_new_figure(title, plot_width=720, plot_height=640, **kwargs):
    """Create a new figure

    Arguments:
        title (str): name of the figure
        plot_width (int): width in pixels
        plot_height (int): height in pixels

    Returns:
        bokeh.plotting.figure.Figure object to be graphed on
    """
    kwargs['plot_width'] = plot_width
    kwargs['plot_height'] = plot_height
    fig = figure(**kwargs)
    return fig


def plot(fig, plot_type, x, y, **kwargs):
    """Create a Glyph Renderer

    Arguments:
        plot_type (str): type of plot to make
        x (list): x data
        y (list): y data
        kwargs: Line properties and Fill properties
    """
    source = ColumnDataSource(data=dict(x=x, y=y))
    if 'color' in kwargs and isinstance(kwargs['color'], list):
        p = palette(max(kwargs['color']))
        source.add([p[i-1] for i in kwargs['color']], 'color')
        del kwargs['color']

    source_keys = {k:k for k in source.data}

    if plot_type == 'line':
        return fig.line(**source_keys, source=source, **kwargs)
    elif plot_type == 'vbar':
        kwargs['width'] = kwargs.get('width', 1)
        return fig.line(**source_keys, source=source, **kwargs)
    else:
        return fig.scatter(**source_keys, marker=plot_type, source=source, **kwargs)

def image(fig, image, **kwargs):
    """
    """
    source = ColumnDataSource(data=dict(image=[image], x=[0], y=[0], dw=[len(image[0])], dh=[len(image)]))
    return fig.image(image='image', x='x', y='y', dw='dw', dh='dh', source=source, **kwargs)


def slider(renderer, title, start, end, **kwargs):
    js_callback_template = 'source.data["{key}"] = {lb}source2.data["{key}"][cb_obj.value]{rb};'
    lb, rb = ['[', ']'] if isinstance(renderer.glyph, bokeh.models.glyphs.Image) else ['','']
    js_callback_source = '\n'.join(js_callback_template.format(key=key, lb=lb, rb=rb) for key in kwargs)
    js_callback_source += '\nsource.change.emit();'
    source2 = ColumnDataSource(data=kwargs)
    slider = Slider(start=start, end=end, value=0, step=1, title=title)
    callback = CustomJS(args=dict(source=renderer.data_source, source2=source2), code=js_callback_source)
    slider.js_on_change('value', callback)
    return slider


def ticker(ticker_type, **kwargs):
    if ticker_type == 'FixedTicker':
        return FixedTicker(**kwargs)
    if ticker_type == 'AdaptiveTicker':
        return AdaptiveTicker(**kwargs)
    if ticker_type == 'CompositeTicker':
        return CompositeTicker(**kwargs)
    if ticker_type == 'SingleIntervalTicker':
        return SingleIntervalTicker(**kwargs)
    if ticker_type == 'DaysTicker':
        return DaysTicker(**kwargs)
    if ticker_type == 'MonthsTicker':
        return MonthsTicker(**kwargs)
    if ticker_type == 'YearsTicker':
        return YearsTicker(**kwargs)
    if ticker_type == 'BasicTicker':
        return BasicTicker(**kwargs)
    if ticker_type == 'LogTicker':
        return LogTicker(**kwargs)
    if ticker_type == 'MercatorTicker':
        return MercatorTicker(**kwargs)
    if ticker_type == 'CategoricalTicker':
        return CategoricalTicker(**kwargs)
    if ticker_type == 'DatetimeTicker':
        return DatetimeTicker(**kwargs)
    else:
        print('Unknown ticker_type ', ticker_type)


def color_mapper(mapper_type, **kwargs):
    if mapper_type == 'CategoricalColorMapper':
        return CategoricalColorMapper(**kwargs)
    if mapper_type == 'CategoricalMarkerMapper':
        return CategoricalMarkerMapper(**kwargs)
    if mapper_type == 'CategoricalPatternMapper':
        return CategoricalPatternMapper(**kwargs)
    if mapper_type == 'LinearColorMapper':
        return LinearColorMapper(**kwargs)
    if mapper_type == 'LogColorMapper':
        return LogColorMapper(**kwargs)
    else:
        print('Unknown mapper_type ', mapper_type)


def color_bar(figure, color_mapper, **kwargs):
    figure.add_layout(ColorBar(color_mapper=color_mapper, **kwargs), 'left')


def layout(obj1, obj2):
    return column(obj1, row(obj2))


def generate_html(obj, filename, **kwargs):
    """Generate and save the HTML of the figure

    Arguments:
        obj: Bokeh object or figure to be graphed
        filename (str): name of the file to save the figure to
        kwargs: additional output properties
    """
    output_file(filename, **kwargs)
    save(obj)

if __name__ == '__main__':
    import random
    x = [random.randint(0,10) for _ in range(100)]
    y = [random.randint(0,10) for _ in range(100)]
    f = make_new_figure('test')
    s = plot(f, 'scatter', x, y)
    generate_html(f, 'test.html')